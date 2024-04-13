gst_asf_demux_check_chained_asf (GstASFDemux * demux)
{
  guint64 off = demux->data_offset + (demux->packet * demux->packet_size);
  GstFlowReturn ret = GST_FLOW_OK;
  GstBuffer *buf = NULL;
  gboolean header = FALSE;

  /* TODO maybe we should skip index objects after the data and look
   * further for a new header */
  if (gst_asf_demux_pull_data (demux, off, ASF_OBJECT_HEADER_SIZE, &buf, &ret)) {
    g_assert (buf != NULL);
    /* check if it is a header */
    if (gst_asf_demux_check_buffer_is_header (demux, buf)) {
      GST_DEBUG_OBJECT (demux, "new base offset: %" G_GUINT64_FORMAT, off);
      demux->base_offset = off;
      header = TRUE;
    }

    gst_buffer_unref (buf);
  }

  return header;
}
