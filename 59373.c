gst_asf_demux_process_bitrate_props_object (GstASFDemux * demux, guint8 * data,
    guint64 size)
{
  guint16 num_streams, i;
  AsfStream *stream;

  if (size < 2)
    goto not_enough_data;

  num_streams = gst_asf_demux_get_uint16 (&data, &size);

  GST_INFO ("object is a bitrate properties object with %u streams",
      num_streams);

  if (size < (num_streams * (2 + 4)))
    goto not_enough_data;

  for (i = 0; i < num_streams; ++i) {
    guint32 bitrate;
    guint16 stream_id;

    stream_id = gst_asf_demux_get_uint16 (&data, &size);
    bitrate = gst_asf_demux_get_uint32 (&data, &size);

    if (stream_id < GST_ASF_DEMUX_NUM_STREAM_IDS) {
      GST_DEBUG_OBJECT (demux, "bitrate of stream %u = %u", stream_id, bitrate);
      stream = gst_asf_demux_get_stream (demux, stream_id);
      if (stream) {
        if (stream->pending_tags == NULL)
          stream->pending_tags = gst_tag_list_new_empty ();
        gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
            GST_TAG_BITRATE, bitrate, NULL);
      } else {
        GST_WARNING_OBJECT (demux, "Stream id %u wasn't found", stream_id);
      }
    } else {
      GST_WARNING ("stream id %u is too large", stream_id);
    }
  }

  return GST_FLOW_OK;

not_enough_data:
  {
    GST_WARNING_OBJECT (demux, "short read parsing bitrate props object!");
    return GST_FLOW_OK;         /* not really fatal */
  }
}
