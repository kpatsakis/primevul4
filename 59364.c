gst_asf_demux_handle_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstASFDemux *demux;
  gboolean ret;

  demux = GST_ASF_DEMUX (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
      GST_LOG_OBJECT (pad, "seek event");
      ret = gst_asf_demux_handle_seek_event (demux, event);
      gst_event_unref (event);
      break;
    case GST_EVENT_QOS:
    case GST_EVENT_NAVIGATION:
      /* just drop these two silently */
      gst_event_unref (event);
      ret = FALSE;
      break;
    default:
      GST_LOG_OBJECT (pad, "%s event", GST_EVENT_TYPE_NAME (event));
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }

  return ret;
}
