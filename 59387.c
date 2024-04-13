gst_asf_demux_release_old_pads (GstASFDemux * demux)
{
  GST_DEBUG_OBJECT (demux, "Releasing old pads");

  while (demux->old_num_streams > 0) {
    gst_pad_push_event (demux->old_stream[demux->old_num_streams - 1].pad,
        gst_event_new_eos ());
    gst_asf_demux_free_stream (demux,
        &demux->old_stream[demux->old_num_streams - 1]);
    --demux->old_num_streams;
  }
  memset (demux->old_stream, 0, sizeof (demux->old_stream));
  demux->old_num_streams = 0;
}
