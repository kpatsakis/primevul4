gst_asf_demux_reset_stream_state_after_discont (GstASFDemux * demux)
{
  guint n;

  gst_adapter_clear (demux->adapter);

  GST_DEBUG_OBJECT (demux, "reset stream state");

  gst_flow_combiner_reset (demux->flowcombiner);
  for (n = 0; n < demux->num_streams; n++) {
    demux->stream[n].discont = TRUE;
    demux->stream[n].first_buffer = TRUE;

    while (demux->stream[n].payloads->len > 0) {
      AsfPayload *payload;
      guint last;

      last = demux->stream[n].payloads->len - 1;
      payload = &g_array_index (demux->stream[n].payloads, AsfPayload, last);
      gst_buffer_replace (&payload->buf, NULL);
      g_array_remove_index (demux->stream[n].payloads, last);
    }
  }
}
