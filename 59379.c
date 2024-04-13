gst_asf_demux_process_metadata (GstASFDemux * demux, guint8 * data,
    guint64 size)
{
  guint16 blockcount, i;

  GST_INFO_OBJECT (demux, "object is a metadata object");

  /* Content Descriptor Count */
  if (size < 2)
    goto not_enough_data;

  blockcount = gst_asf_demux_get_uint16 (&data, &size);

  for (i = 0; i < blockcount; ++i) {
    GstStructure *s;
    guint16 stream_num, name_len, data_type, lang_idx G_GNUC_UNUSED;
    guint32 data_len, ival;
    gchar *name_utf8;

    if (size < (2 + 2 + 2 + 2 + 4))
      goto not_enough_data;

    lang_idx = gst_asf_demux_get_uint16 (&data, &size);
    stream_num = gst_asf_demux_get_uint16 (&data, &size);
    name_len = gst_asf_demux_get_uint16 (&data, &size);
    data_type = gst_asf_demux_get_uint16 (&data, &size);
    data_len = gst_asf_demux_get_uint32 (&data, &size);

    if (size < name_len + data_len)
      goto not_enough_data;

    /* convert name to UTF-8 */
    name_utf8 = g_convert ((gchar *) data, name_len, "UTF-8", "UTF-16LE",
        NULL, NULL, NULL);
    gst_asf_demux_skip_bytes (name_len, &data, &size);

    if (name_utf8 == NULL) {
      GST_WARNING ("Failed to convert value name to UTF8, skipping");
      gst_asf_demux_skip_bytes (data_len, &data, &size);
      continue;
    }

    if (data_type != ASF_DEMUX_DATA_TYPE_DWORD) {
      gst_asf_demux_skip_bytes (data_len, &data, &size);
      g_free (name_utf8);
      continue;
    }

    /* read DWORD */
    if (size < 4) {
      g_free (name_utf8);
      goto not_enough_data;
    }

    ival = gst_asf_demux_get_uint32 (&data, &size);

    /* skip anything else there may be, just in case */
    gst_asf_demux_skip_bytes (data_len - 4, &data, &size);

    s = gst_asf_demux_get_metadata_for_stream (demux, stream_num);
    gst_structure_set (s, name_utf8, G_TYPE_INT, ival, NULL);
    g_free (name_utf8);
  }

  GST_INFO_OBJECT (demux, "metadata = %" GST_PTR_FORMAT, demux->metadata);
  return GST_FLOW_OK;

  /* Errors */
not_enough_data:
  {
    GST_WARNING ("Unexpected end of data parsing metadata object");
    return GST_FLOW_OK;         /* not really fatal */
  }
}
