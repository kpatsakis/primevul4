gst_asf_demux_process_ext_stream_props (GstASFDemux * demux, guint8 * data,
    guint64 size)
{
  AsfStreamExtProps esp;
  AsfStream *stream = NULL;
  AsfObject stream_obj;
  guint16 stream_name_count;
  guint16 num_payload_ext;
  guint64 len;
  guint8 *stream_obj_data = NULL;
  guint8 *data_start;
  guint obj_size;
  guint i, stream_num;

  data_start = data;
  obj_size = (guint) size;

  esp.payload_extensions = NULL;

  if (size < 64)
    goto not_enough_data;

  esp.valid = TRUE;
  esp.start_time = gst_asf_demux_get_uint64 (&data, &size) * GST_MSECOND;
  esp.end_time = gst_asf_demux_get_uint64 (&data, &size) * GST_MSECOND;
  esp.data_bitrate = gst_asf_demux_get_uint32 (&data, &size);
  esp.buffer_size = gst_asf_demux_get_uint32 (&data, &size);
  esp.intial_buf_fullness = gst_asf_demux_get_uint32 (&data, &size);
  esp.data_bitrate2 = gst_asf_demux_get_uint32 (&data, &size);
  esp.buffer_size2 = gst_asf_demux_get_uint32 (&data, &size);
  esp.intial_buf_fullness2 = gst_asf_demux_get_uint32 (&data, &size);
  esp.max_obj_size = gst_asf_demux_get_uint32 (&data, &size);
  esp.flags = gst_asf_demux_get_uint32 (&data, &size);
  stream_num = gst_asf_demux_get_uint16 (&data, &size);
  esp.lang_idx = gst_asf_demux_get_uint16 (&data, &size);
  esp.avg_time_per_frame = gst_asf_demux_get_uint64 (&data, &size);
  stream_name_count = gst_asf_demux_get_uint16 (&data, &size);
  num_payload_ext = gst_asf_demux_get_uint16 (&data, &size);

  GST_INFO ("start_time             = %" GST_TIME_FORMAT,
      GST_TIME_ARGS (esp.start_time));
  GST_INFO ("end_time               = %" GST_TIME_FORMAT,
      GST_TIME_ARGS (esp.end_time));
  GST_INFO ("flags                  = %08x", esp.flags);
  GST_INFO ("average time per frame = %" GST_TIME_FORMAT,
      GST_TIME_ARGS (esp.avg_time_per_frame * 100));
  GST_INFO ("stream number          = %u", stream_num);
  GST_INFO ("stream language ID idx = %u (%s)", esp.lang_idx,
      (esp.lang_idx < demux->num_languages) ?
      GST_STR_NULL (demux->languages[esp.lang_idx]) : "??");
  GST_INFO ("stream name count      = %u", stream_name_count);

  /* read stream names */
  for (i = 0; i < stream_name_count; ++i) {
    guint16 stream_lang_idx G_GNUC_UNUSED;
    gchar *stream_name = NULL;

    if (size < 2)
      goto not_enough_data;
    stream_lang_idx = gst_asf_demux_get_uint16 (&data, &size);
    if (!gst_asf_demux_get_string (&stream_name, NULL, &data, &size))
      goto not_enough_data;
    GST_INFO ("stream name %d: %s", i, GST_STR_NULL (stream_name));
    g_free (stream_name);       /* TODO: store names in struct */
  }

  /* read payload extension systems stuff */
  GST_LOG ("payload extension systems count = %u", num_payload_ext);

  if (num_payload_ext > 0)
    esp.payload_extensions = g_new0 (AsfPayloadExtension, num_payload_ext + 1);

  for (i = 0; i < num_payload_ext; ++i) {
    AsfPayloadExtension ext;
    ASFGuid ext_guid;
    guint32 sys_info_len;

    if (size < 16 + 2 + 4)
      goto not_enough_data;

    gst_asf_demux_get_guid (&ext_guid, &data, &size);
    ext.id = gst_asf_demux_identify_guid (asf_payload_ext_guids, &ext_guid);
    ext.len = gst_asf_demux_get_uint16 (&data, &size);

    sys_info_len = gst_asf_demux_get_uint32 (&data, &size);
    GST_LOG ("payload systems info len = %u", sys_info_len);
    if (!gst_asf_demux_skip_bytes (sys_info_len, &data, &size))
      goto not_enough_data;

    esp.payload_extensions[i] = ext;
  }

  GST_LOG ("bytes read: %u/%u", (guint) (data - data_start), obj_size);

  /* there might be an optional STREAM_INFO object here now; if not, we
   * should have parsed the corresponding stream info object already (since
   * we are parsing the extended stream properties objects delayed) */
  if (size == 0) {
    stream = gst_asf_demux_get_stream (demux, stream_num);
    goto done;
  }

  if (size < ASF_OBJECT_HEADER_SIZE)
    goto not_enough_data;

  /* get size of the stream object */
  if (!asf_demux_peek_object (demux, data, size, &stream_obj, TRUE))
    goto corrupted_stream;

  if (stream_obj.id != ASF_OBJ_STREAM)
    goto expected_stream_object;

  if (stream_obj.size < ASF_OBJECT_HEADER_SIZE ||
      stream_obj.size > (10 * 1024 * 1024))
    goto not_enough_data;

  gst_asf_demux_skip_bytes (ASF_OBJECT_HEADER_SIZE, &data, &size);

  /* process this stream object later after all the other 'normal' ones
   * have been processed (since the others are more important/non-hidden) */
  len = stream_obj.size - ASF_OBJECT_HEADER_SIZE;
  if (!gst_asf_demux_get_bytes (&stream_obj_data, len, &data, &size))
    goto not_enough_data;

  /* parse stream object */
  stream = gst_asf_demux_parse_stream_object (demux, stream_obj_data, len);
  g_free (stream_obj_data);

done:

  if (stream) {
    stream->ext_props = esp;

    /* try to set the framerate */
    if (stream->is_video && stream->caps) {
      GValue framerate = { 0 };
      GstStructure *s;
      gint num, denom;

      g_value_init (&framerate, GST_TYPE_FRACTION);

      num = GST_SECOND / 100;
      denom = esp.avg_time_per_frame;
      if (denom == 0) {
        /* avoid division by 0, assume 25/1 framerate */
        denom = GST_SECOND / 2500;
      }

      gst_value_set_fraction (&framerate, num, denom);

      stream->caps = gst_caps_make_writable (stream->caps);
      s = gst_caps_get_structure (stream->caps, 0);
      gst_structure_set_value (s, "framerate", &framerate);
      g_value_unset (&framerate);
      GST_DEBUG_OBJECT (demux, "setting framerate of %d/%d = %f",
          num, denom, ((gdouble) num) / denom);
    }

    /* add language info now if we have it */
    if (stream->ext_props.lang_idx < demux->num_languages) {
      if (stream->pending_tags == NULL)
        stream->pending_tags = gst_tag_list_new_empty ();
      GST_LOG_OBJECT (demux, "stream %u has language '%s'", stream->id,
          demux->languages[stream->ext_props.lang_idx]);
      gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_APPEND,
          GST_TAG_LANGUAGE_CODE, demux->languages[stream->ext_props.lang_idx],
          NULL);
    }
  } else if (gst_asf_demux_is_unknown_stream (demux, stream_num)) {
    GST_WARNING_OBJECT (demux, "Ext. stream properties for unknown stream");
  }

  if (!stream)
    g_free (esp.payload_extensions);

  return GST_FLOW_OK;

  /* Errors */
not_enough_data:
  {
    GST_WARNING_OBJECT (demux, "short read parsing ext stream props object!");
    g_free (esp.payload_extensions);
    return GST_FLOW_OK;         /* not absolutely fatal */
  }
expected_stream_object:
  {
    GST_WARNING_OBJECT (demux, "error parsing extended stream properties "
        "object: expected embedded stream object, but got %s object instead!",
        gst_asf_get_guid_nick (asf_object_guids, stream_obj.id));
    g_free (esp.payload_extensions);
    return GST_FLOW_OK;         /* not absolutely fatal */
  }
corrupted_stream:
  {
    GST_WARNING_OBJECT (demux, "Corrupted stream");
    g_free (esp.payload_extensions);
    return GST_FLOW_ERROR;
  }
}
