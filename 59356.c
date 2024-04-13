gst_asf_demux_get_stream (GstASFDemux * demux, guint16 id)
{
  guint i;

  for (i = 0; i < demux->num_streams; i++) {
    if (demux->stream[i].id == id)
      return &demux->stream[i];
  }

  if (gst_asf_demux_is_unknown_stream (demux, id))
    GST_WARNING ("Segment found for undefined stream: (%d)", id);
  return NULL;
}
