gst_asf_demux_check_activate_streams (GstASFDemux * demux, gboolean force)
{
  guint i, actual_streams = 0;

  if (demux->activated_streams)
    return TRUE;

  if (G_UNLIKELY (!gst_asf_demux_check_first_ts (demux, force)))
    return FALSE;

  if (!all_streams_prerolled (demux) && !force) {
    GST_DEBUG_OBJECT (demux, "not all streams with data beyond preroll yet");
    return FALSE;
  }

  for (i = 0; i < demux->num_streams; ++i) {
    AsfStream *stream = &demux->stream[i];

    if (stream->payloads->len > 0) {

      if (stream->inspect_payload &&    /* dvr-ms required payload inspection */
          !stream->active &&    /* do not inspect active streams (caps were already set) */
          !gst_asf_demux_update_caps_from_payload (demux, stream) &&    /* failed to determine caps */
          stream->payloads->len < 20) { /* if we couldn't determine the caps from 20 packets then just give up and use whatever was in codecTag */
        /* try to gather some more data  */
        return FALSE;
      }
      /* we don't check mutual exclusion stuff here; either we have data for
       * a stream, then we active it, or we don't, then we'll ignore it */
      GST_LOG_OBJECT (stream->pad, "is prerolled - activate!");
      gst_asf_demux_activate_stream (demux, stream);
      actual_streams += 1;
    } else {
      GST_LOG_OBJECT (stream->pad, "no data, ignoring stream");
    }
  }

  if (actual_streams == 0) {
    /* We don't have any streams activated ! */
    GST_ERROR_OBJECT (demux, "No streams activated!");
    return FALSE;
  }

  gst_asf_demux_release_old_pads (demux);

  demux->activated_streams = TRUE;
  GST_LOG_OBJECT (demux, "signalling no more pads");
  gst_element_no_more_pads (GST_ELEMENT (demux));
  return TRUE;
}
