gst_asf_demux_seek_index_lookup (GstASFDemux * demux, guint * packet,
    GstClockTime seek_time, GstClockTime * p_idx_time, guint * speed,
    gboolean next, gboolean * eos)
{
  GstClockTime idx_time;
  guint idx;

  if (eos)
    *eos = FALSE;

  if (G_UNLIKELY (demux->sidx_num_entries == 0 || demux->sidx_interval == 0))
    return FALSE;

  idx = (guint) ((seek_time + demux->preroll) / demux->sidx_interval);

  if (next) {
    /* if we want the next keyframe, we have to go forward till we find
       a different packet number */
    guint idx2;
    if (idx >= demux->sidx_num_entries - 1) {
      /* If we get here, we're asking for next keyframe after the last one. There isn't one. */
      if (eos)
        *eos = TRUE;
      return FALSE;
    }
    for (idx2 = idx + 1; idx2 < demux->sidx_num_entries; ++idx2) {
      if (demux->sidx_entries[idx].packet != demux->sidx_entries[idx2].packet) {
        idx = idx2;
        break;
      }
    }
  }

  if (G_UNLIKELY (idx >= demux->sidx_num_entries)) {
    if (eos)
      *eos = TRUE;
    return FALSE;
  }

  *packet = demux->sidx_entries[idx].packet;
  if (speed)
    *speed = demux->sidx_entries[idx].count;

  /* so we get closer to the actual time of the packet ... actually, let's not
   * do this, since we throw away superfluous payloads before the seek position
   * anyway; this way, our key unit seek 'snap resolution' is a bit better
   * (ie. same as index resolution) */
  /*
     while (idx > 0 && demux->sidx_entries[idx-1] == demux->sidx_entries[idx])
     --idx;
   */

  idx_time = demux->sidx_interval * idx;
  if (G_LIKELY (idx_time >= demux->preroll))
    idx_time -= demux->preroll;

  GST_DEBUG_OBJECT (demux, "%" GST_TIME_FORMAT " => packet %u at %"
      GST_TIME_FORMAT, GST_TIME_ARGS (seek_time), *packet,
      GST_TIME_ARGS (idx_time));

  if (G_LIKELY (p_idx_time))
    *p_idx_time = idx_time;

  return TRUE;
}
