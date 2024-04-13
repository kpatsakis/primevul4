gst_asf_demux_parse_stream_object (GstASFDemux * demux, guint8 * data,
    guint64 size)
{
  AsfCorrectionType correction_type;
  AsfStreamType stream_type;
  GstClockTime time_offset;
  gboolean is_encrypted G_GNUC_UNUSED;
  guint16 stream_id;
  guint16 flags;
  ASFGuid guid;
  guint stream_specific_size;
  guint type_specific_size G_GNUC_UNUSED;
  guint unknown G_GNUC_UNUSED;
  gboolean inspect_payload = FALSE;
  AsfStream *stream = NULL;

  /* Get the rest of the header's header */
  if (size < (16 + 16 + 8 + 4 + 4 + 2 + 4))
    goto not_enough_data;

  gst_asf_demux_get_guid (&guid, &data, &size);
  stream_type = gst_asf_demux_identify_guid (asf_stream_guids, &guid);

  gst_asf_demux_get_guid (&guid, &data, &size);
  correction_type = gst_asf_demux_identify_guid (asf_correction_guids, &guid);

  time_offset = gst_asf_demux_get_uint64 (&data, &size) * 100;

  type_specific_size = gst_asf_demux_get_uint32 (&data, &size);
  stream_specific_size = gst_asf_demux_get_uint32 (&data, &size);

  flags = gst_asf_demux_get_uint16 (&data, &size);
  stream_id = flags & 0x7f;
  is_encrypted = ! !((flags & 0x8000) << 15);
  unknown = gst_asf_demux_get_uint32 (&data, &size);

  GST_DEBUG_OBJECT (demux, "Found stream %u, time_offset=%" GST_TIME_FORMAT,
      stream_id, GST_TIME_ARGS (time_offset));

  /* dvr-ms has audio stream declared in stream specific data */
  if (stream_type == ASF_STREAM_EXT_EMBED_HEADER) {
    AsfExtStreamType ext_stream_type;
    gst_asf_demux_get_guid (&guid, &data, &size);
    ext_stream_type = gst_asf_demux_identify_guid (asf_ext_stream_guids, &guid);

    if (ext_stream_type == ASF_EXT_STREAM_AUDIO) {
      inspect_payload = TRUE;

      gst_asf_demux_get_guid (&guid, &data, &size);
      gst_asf_demux_get_uint32 (&data, &size);
      gst_asf_demux_get_uint32 (&data, &size);
      gst_asf_demux_get_uint32 (&data, &size);
      gst_asf_demux_get_guid (&guid, &data, &size);
      gst_asf_demux_get_uint32 (&data, &size);
      stream_type = ASF_STREAM_AUDIO;
    }
  }

  switch (stream_type) {
    case ASF_STREAM_AUDIO:{
      asf_stream_audio audio_object;

      if (!gst_asf_demux_get_stream_audio (&audio_object, &data, &size))
        goto not_enough_data;

      GST_INFO ("Object is an audio stream with %u bytes of additional data",
          audio_object.size);

      stream = gst_asf_demux_add_audio_stream (demux, &audio_object, stream_id,
          &data, &size);

      switch (correction_type) {
        case ASF_CORRECTION_ON:{
          guint span, packet_size, chunk_size, data_size, silence_data;

          GST_INFO ("Using error correction");

          if (size < (1 + 2 + 2 + 2 + 1))
            goto not_enough_data;

          span = gst_asf_demux_get_uint8 (&data, &size);
          packet_size = gst_asf_demux_get_uint16 (&data, &size);
          chunk_size = gst_asf_demux_get_uint16 (&data, &size);
          data_size = gst_asf_demux_get_uint16 (&data, &size);
          silence_data = gst_asf_demux_get_uint8 (&data, &size);

          stream->span = span;

          GST_DEBUG_OBJECT (demux, "Descrambling ps:%u cs:%u ds:%u s:%u sd:%u",
              packet_size, chunk_size, data_size, span, silence_data);

          if (stream->span > 1) {
            if (chunk_size == 0 || ((packet_size / chunk_size) <= 1)) {
              /* Disable descrambling */
              stream->span = 0;
            } else {
              /* FIXME: this else branch was added for
               * weird_al_yankovic - the saga begins.asf */
              stream->ds_packet_size = packet_size;
              stream->ds_chunk_size = chunk_size;
            }
          } else {
            /* Descambling is enabled */
            stream->ds_packet_size = packet_size;
            stream->ds_chunk_size = chunk_size;
          }
#if 0
          /* Now skip the rest of the silence data */
          if (data_size > 1)
            gst_bytestream_flush (demux->bs, data_size - 1);
#else
          /* FIXME: CHECKME. And why -1? */
          if (data_size > 1) {
            if (!gst_asf_demux_skip_bytes (data_size - 1, &data, &size)) {
              goto not_enough_data;
            }
          }
#endif
          break;
        }
        case ASF_CORRECTION_OFF:{
          GST_INFO ("Error correction off");
          if (!gst_asf_demux_skip_bytes (stream_specific_size, &data, &size))
            goto not_enough_data;
          break;
        }
        default:
          GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
              ("Audio stream using unknown error correction"));
          return NULL;
      }

      break;
    }

    case ASF_STREAM_VIDEO:{
      asf_stream_video_format video_format_object;
      asf_stream_video video_object;
      guint16 vsize;

      if (!gst_asf_demux_get_stream_video (&video_object, &data, &size))
        goto not_enough_data;

      vsize = video_object.size - 40;   /* Byte order gets offset by single byte */

      GST_INFO ("object is a video stream with %u bytes of "
          "additional data", vsize);

      if (!gst_asf_demux_get_stream_video_format (&video_format_object,
              &data, &size)) {
        goto not_enough_data;
      }

      stream = gst_asf_demux_add_video_stream (demux, &video_format_object,
          stream_id, &data, &size);

      break;
    }

    default:
      GST_WARNING_OBJECT (demux, "Unknown stream type for stream %u",
          stream_id);
      demux->other_streams =
          g_slist_append (demux->other_streams, GINT_TO_POINTER (stream_id));
      break;
  }

  if (stream)
    stream->inspect_payload = inspect_payload;
  return stream;

not_enough_data:
  {
    GST_WARNING_OBJECT (demux, "Unexpected end of data parsing stream object");
    /* we'll error out later if we found no streams */
    return NULL;
  }
}
