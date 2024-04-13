gst_asf_demux_init (GstASFDemux * demux)
{
  demux->sinkpad =
      gst_pad_new_from_static_template (&gst_asf_demux_sink_template, "sink");
  gst_pad_set_chain_function (demux->sinkpad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_chain));
  gst_pad_set_event_function (demux->sinkpad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_sink_event));
  gst_pad_set_activate_function (demux->sinkpad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_activate));
  gst_pad_set_activatemode_function (demux->sinkpad,
      GST_DEBUG_FUNCPTR (gst_asf_demux_activate_mode));
  gst_element_add_pad (GST_ELEMENT (demux), demux->sinkpad);

  /* set initial state */
  gst_asf_demux_reset (demux, FALSE);
}
