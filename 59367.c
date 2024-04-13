gst_asf_demux_is_unknown_stream (GstASFDemux * demux, guint stream_num)
{
  return g_slist_find (demux->other_streams,
      GINT_TO_POINTER (stream_num)) == NULL;
}
