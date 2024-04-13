gst_asf_demux_handle_seek_push (GstASFDemux * demux, GstEvent * event)
{
  gdouble rate;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  gint64 cur, stop;
  guint packet;
  gboolean res;
  GstEvent *byte_event;

  gst_event_parse_seek (event, &rate, &format, &flags, &cur_type, &cur,
      &stop_type, &stop);

  stop_type = GST_SEEK_TYPE_NONE;
  stop = -1;

  GST_DEBUG_OBJECT (demux, "seeking to %" GST_TIME_FORMAT, GST_TIME_ARGS (cur));

  /* determine packet, by index or by estimation */
  if (!gst_asf_demux_seek_index_lookup (demux, &packet, cur, NULL, NULL, FALSE,
          NULL)) {
    packet =
        (guint) gst_util_uint64_scale (demux->num_packets, cur,
        demux->play_time);
  }

  if (packet > demux->num_packets) {
    GST_DEBUG_OBJECT (demux, "could not determine packet to seek to, "
        "seek aborted.");
    return FALSE;
  }

  GST_DEBUG_OBJECT (demux, "seeking to packet %d", packet);

  cur = demux->data_offset + ((guint64) packet * demux->packet_size);

  GST_DEBUG_OBJECT (demux, "Pushing BYTE seek rate %g, "
      "start %" G_GINT64_FORMAT ", stop %" G_GINT64_FORMAT, rate, cur, stop);
  /* BYTE seek event */
  byte_event = gst_event_new_seek (rate, GST_FORMAT_BYTES, flags, cur_type,
      cur, stop_type, stop);
  gst_event_set_seqnum (byte_event, gst_event_get_seqnum (event));
  res = gst_pad_push_event (demux->sinkpad, byte_event);

  return res;
}
