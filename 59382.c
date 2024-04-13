gst_asf_demux_pull_data (GstASFDemux * demux, guint64 offset, guint size,
    GstBuffer ** p_buf, GstFlowReturn * p_flow)
{
  gsize buffer_size;
  GstFlowReturn flow;

  GST_LOG_OBJECT (demux, "pulling buffer at %" G_GUINT64_FORMAT "+%u",
      offset, size);

  flow = gst_pad_pull_range (demux->sinkpad, offset, size, p_buf);

  if (G_LIKELY (p_flow))
    *p_flow = flow;

  if (G_UNLIKELY (flow != GST_FLOW_OK)) {
    GST_DEBUG_OBJECT (demux, "flow %s pulling buffer at %" G_GUINT64_FORMAT
        "+%u", gst_flow_get_name (flow), offset, size);
    *p_buf = NULL;
    return FALSE;
  }

  g_assert (*p_buf != NULL);

  buffer_size = gst_buffer_get_size (*p_buf);
  if (G_UNLIKELY (buffer_size < size)) {
    GST_DEBUG_OBJECT (demux, "short read pulling buffer at %" G_GUINT64_FORMAT
        "+%u (got only %" G_GSIZE_FORMAT " bytes)", offset, size, buffer_size);
    gst_buffer_unref (*p_buf);
    if (G_LIKELY (p_flow))
      *p_flow = GST_FLOW_EOS;
    *p_buf = NULL;
    return FALSE;
  }

  return TRUE;
}
