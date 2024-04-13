gst_asf_demux_send_event_unlocked (GstASFDemux * demux, GstEvent * event)
{
  gboolean ret = TRUE;
  gint i;

  GST_DEBUG_OBJECT (demux, "sending %s event to all source pads",
      GST_EVENT_TYPE_NAME (event));

  for (i = 0; i < demux->num_streams; ++i) {
    gst_event_ref (event);
    ret &= gst_pad_push_event (demux->stream[i].pad, event);
  }
  gst_event_unref (event);
  return ret;
}
