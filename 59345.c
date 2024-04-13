gst_asf_demux_check_first_ts (GstASFDemux * demux, gboolean force)
{
  if (G_UNLIKELY (!GST_CLOCK_TIME_IS_VALID (demux->first_ts))) {
    GstClockTime first_ts = GST_CLOCK_TIME_NONE;
    int i;

    /* go trhough each stream, find smallest timestamp */
    for (i = 0; i < demux->num_streams; ++i) {
      AsfStream *stream;
      int j;
      GstClockTime stream_min_ts = GST_CLOCK_TIME_NONE;
      GstClockTime stream_min_ts2 = GST_CLOCK_TIME_NONE;        /* second smallest timestamp */
      stream = &demux->stream[i];

      for (j = 0; j < stream->payloads->len; ++j) {
        AsfPayload *payload = &g_array_index (stream->payloads, AsfPayload, j);
        if (GST_CLOCK_TIME_IS_VALID (payload->ts) &&
            (!GST_CLOCK_TIME_IS_VALID (stream_min_ts)
                || stream_min_ts > payload->ts)) {
          stream_min_ts = payload->ts;
        }
        if (GST_CLOCK_TIME_IS_VALID (payload->ts) &&
            payload->ts > stream_min_ts &&
            (!GST_CLOCK_TIME_IS_VALID (stream_min_ts2)
                || stream_min_ts2 > payload->ts)) {
          stream_min_ts2 = payload->ts;
        }
      }

      /* there are some DVR ms files where first packet has TS of 0 (instead of -1) while subsequent packets have
         regular (singificantly larger) timestamps. If we don't deal with it, we may end up with huge gap in timestamps
         which makes playback stuck. The 0 timestamp may also be valid though, if the second packet timestamp continues 
         from it. I havent found a better way to distinguish between these two, except to set an arbitrary boundary
         and disregard the first 0 timestamp if the second timestamp is bigger than the boundary) */

      if (stream_min_ts == 0 && stream_min_ts2 == GST_CLOCK_TIME_NONE && !force)        /* still waiting for the second timestamp */
        return FALSE;

      if (stream_min_ts == 0 && stream_min_ts2 > GST_SECOND)    /* first timestamp is 0 and second is significantly larger, disregard the 0 */
        stream_min_ts = stream_min_ts2;

      /* if we don't have timestamp for this stream, wait for more data */
      if (!GST_CLOCK_TIME_IS_VALID (stream_min_ts) && !force)
        return FALSE;

      if (GST_CLOCK_TIME_IS_VALID (stream_min_ts) &&
          (!GST_CLOCK_TIME_IS_VALID (first_ts) || first_ts > stream_min_ts))
        first_ts = stream_min_ts;
    }

    if (!GST_CLOCK_TIME_IS_VALID (first_ts))    /* can happen with force = TRUE */
      first_ts = 0;

    demux->first_ts = first_ts;

    /* update packets queued before we knew first timestamp */
    for (i = 0; i < demux->num_streams; ++i) {
      AsfStream *stream;
      int j;
      stream = &demux->stream[i];

      for (j = 0; j < stream->payloads->len; ++j) {
        AsfPayload *payload = &g_array_index (stream->payloads, AsfPayload, j);
        if (GST_CLOCK_TIME_IS_VALID (payload->ts)) {
          if (payload->ts > first_ts)
            payload->ts -= first_ts;
          else
            payload->ts = 0;
        }
      }
    }
  }

  gst_asf_demux_check_segment_ts (demux, 0);

  return TRUE;
}
