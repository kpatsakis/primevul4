gst_asf_demux_handle_seek_event (GstASFDemux * demux, GstEvent * event)
{
  GstClockTime idx_time;
  GstSegment segment;
  GstSeekFlags flags;
  GstSeekType cur_type, stop_type;
  GstFormat format;
  gboolean only_need_update;
  gboolean after, before, next;
  gboolean flush;
  gdouble rate;
  gint64 cur, stop;
  gint64 seek_time;
  guint packet, speed_count = 1;
  gboolean eos;
  guint32 seqnum;
  GstEvent *fevent;
  gint i;

  gst_event_parse_seek (event, &rate, &format, &flags, &cur_type, &cur,
      &stop_type, &stop);

  if (G_UNLIKELY (format != GST_FORMAT_TIME)) {
    GST_LOG_OBJECT (demux, "seeking is only supported in TIME format");
    return FALSE;
  }

  /* upstream might handle TIME seek, e.g. mms or rtsp, or not, e.g. http,
   * so first try to let it handle the seek event. */
  if (gst_pad_push_event (demux->sinkpad, gst_event_ref (event)))
    return TRUE;

  if (G_UNLIKELY (demux->seekable == FALSE || demux->packet_size == 0 ||
          demux->num_packets == 0 || demux->play_time == 0)) {
    GST_LOG_OBJECT (demux, "stream is not seekable");
    return FALSE;
  }

  if (G_UNLIKELY (!demux->activated_streams)) {
    GST_LOG_OBJECT (demux, "streams not yet activated, ignoring seek");
    return FALSE;
  }

  if (G_UNLIKELY (rate <= 0.0)) {
    GST_LOG_OBJECT (demux, "backward playback");
    demux->seek_to_cur_pos = TRUE;
    for (i = 0; i < demux->num_streams; i++) {
      demux->stream[i].reverse_kf_ready = FALSE;
    }
  }

  seqnum = gst_event_get_seqnum (event);
  flush = ((flags & GST_SEEK_FLAG_FLUSH) == GST_SEEK_FLAG_FLUSH);
  demux->accurate =
      ((flags & GST_SEEK_FLAG_ACCURATE) == GST_SEEK_FLAG_ACCURATE);
  demux->keyunit_sync =
      ((flags & GST_SEEK_FLAG_KEY_UNIT) == GST_SEEK_FLAG_KEY_UNIT);
  after = ((flags & GST_SEEK_FLAG_SNAP_AFTER) == GST_SEEK_FLAG_SNAP_AFTER);
  before = ((flags & GST_SEEK_FLAG_SNAP_BEFORE) == GST_SEEK_FLAG_SNAP_BEFORE);
  next = after && !before;

  if (G_UNLIKELY (demux->streaming)) {
    /* support it safely needs more segment handling, e.g. closing etc */
    if (!flush) {
      GST_LOG_OBJECT (demux, "streaming; non-flushing seek not supported");
      return FALSE;
    }
    /* we can (re)construct the start later on, but not the end */
    if (stop_type != GST_SEEK_TYPE_NONE &&
        (stop_type != GST_SEEK_TYPE_SET || GST_CLOCK_TIME_IS_VALID (stop))) {
      GST_LOG_OBJECT (demux, "streaming; end position must be NONE");
      return FALSE;
    }
    return gst_asf_demux_handle_seek_push (demux, event);
  }

  /* unlock the streaming thread */
  if (G_LIKELY (flush)) {
    fevent = gst_event_new_flush_start ();

    gst_event_set_seqnum (fevent, seqnum);
    gst_pad_push_event (demux->sinkpad, gst_event_ref (fevent));
    gst_asf_demux_send_event_unlocked (demux, fevent);
  } else {
    gst_pad_pause_task (demux->sinkpad);
  }

  /* grab the stream lock so that streaming cannot continue, for
   * non flushing seeks when the element is in PAUSED this could block
   * forever */
  GST_PAD_STREAM_LOCK (demux->sinkpad);

  /* we now can stop flushing, since we have the stream lock now */
  fevent = gst_event_new_flush_stop (TRUE);
  gst_event_set_seqnum (fevent, seqnum);
  gst_pad_push_event (demux->sinkpad, gst_event_ref (fevent));

  if (G_LIKELY (flush))
    gst_asf_demux_send_event_unlocked (demux, fevent);
  else
    gst_event_unref (fevent);

  /* operating on copy of segment until we know the seek worked */
  segment = demux->segment;

  if (G_UNLIKELY (demux->segment_running && !flush)) {
    GstSegment newsegment;
    GstEvent *newseg;

    /* create the segment event to close the current segment */
    gst_segment_copy_into (&segment, &newsegment);
    newseg = gst_event_new_segment (&newsegment);
    gst_event_set_seqnum (newseg, seqnum);

    gst_asf_demux_send_event_unlocked (demux, newseg);
  }

  gst_segment_do_seek (&segment, rate, format, flags, cur_type,
      cur, stop_type, stop, &only_need_update);

  GST_DEBUG_OBJECT (demux, "seeking to time %" GST_TIME_FORMAT ", segment: "
      "%" GST_SEGMENT_FORMAT, GST_TIME_ARGS (segment.start), &segment);

  if (cur_type != GST_SEEK_TYPE_SET)
    seek_time = segment.start;
  else
    seek_time = cur;

  /* FIXME: should check the KEY_UNIT flag; need to adjust position to
   * real start of data and segment_start to indexed time for key unit seek*/
  if (G_UNLIKELY (!gst_asf_demux_seek_index_lookup (demux, &packet, seek_time,
              &idx_time, &speed_count, next, &eos))) {
    gint64 offset;

    if (eos) {
      demux->packet = demux->num_packets;
      goto skip;
    }

    /* First try to query our source to see if it can convert for us. This is
       the case when our source is an mms stream, notice that in this case
       gstmms will do a time based seek to get the byte offset, this is not a
       problem as the seek to this offset needs to happen anway. */
    if (gst_pad_peer_query_convert (demux->sinkpad, GST_FORMAT_TIME, seek_time,
            GST_FORMAT_BYTES, &offset)) {
      packet = (offset - demux->data_offset) / demux->packet_size;
      GST_LOG_OBJECT (demux, "convert %" GST_TIME_FORMAT
          " to bytes query result: %" G_GINT64_FORMAT ", data_ofset: %"
          G_GINT64_FORMAT ", packet_size: %u," " resulting packet: %u\n",
          GST_TIME_ARGS (seek_time), offset, demux->data_offset,
          demux->packet_size, packet);
    } else {
      /* FIXME: For streams containing video, seek to an earlier position in
       * the hope of hitting a keyframe and let the sinks throw away the stuff
       * before the segment start. For audio-only this is unnecessary as every
       * frame is 'key'. */
      if (flush && (demux->accurate || (demux->keyunit_sync && !next))
          && demux->num_video_streams > 0) {
        seek_time -= 5 * GST_SECOND;
        if (seek_time < 0)
          seek_time = 0;
      }

      packet = (guint) gst_util_uint64_scale (demux->num_packets,
          seek_time, demux->play_time);

      if (packet > demux->num_packets)
        packet = demux->num_packets;
    }
  } else {
    if (G_LIKELY (demux->keyunit_sync && !demux->accurate)) {
      GST_DEBUG_OBJECT (demux, "key unit seek, adjust seek_time = %"
          GST_TIME_FORMAT " to index_time = %" GST_TIME_FORMAT,
          GST_TIME_ARGS (seek_time), GST_TIME_ARGS (idx_time));
      segment.start = idx_time;
      segment.position = idx_time;
      segment.time = idx_time;
    }
  }

  GST_DEBUG_OBJECT (demux, "seeking to packet %u (%d)", packet, speed_count);

  GST_OBJECT_LOCK (demux);
  demux->segment = segment;
  if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)) {
    demux->packet = (gint64) gst_util_uint64_scale (demux->num_packets,
        stop, demux->play_time);
  } else {
    demux->packet = packet;
  }

  demux->need_newsegment = TRUE;
  demux->segment_seqnum = seqnum;
  demux->speed_packets =
      GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment) ? 1 : speed_count;
  gst_asf_demux_reset_stream_state_after_discont (demux);
  GST_OBJECT_UNLOCK (demux);

skip:
  /* restart our task since it might have been stopped when we did the flush */
  gst_pad_start_task (demux->sinkpad, (GstTaskFunction) gst_asf_demux_loop,
      demux, NULL);

  /* streaming can continue now */
  GST_PAD_STREAM_UNLOCK (demux->sinkpad);

  return TRUE;
}
