gst_asf_demux_mark_discont (GstASFDemux * demux)
{
  guint n;

  GST_DEBUG_OBJECT (demux, "Mark stream discont");

  for (n = 0; n < demux->num_streams; n++)
    demux->stream[n].discont = TRUE;
}
