gst_asf_demux_push_complete_payloads (GstASFDemux * demux, gboolean force)
{
  AsfStream *stream;
  GstFlowReturn ret = GST_FLOW_OK;

  if (G_UNLIKELY (!demux->activated_streams)) {
    if (!gst_asf_demux_check_activate_streams (demux, force))
      return GST_FLOW_OK;
    /* streams are now activated */
  }

  while ((stream = gst_asf_demux_find_stream_with_complete_payload (demux))) {
    AsfPayload *payload;
    GstClockTime timestamp = GST_CLOCK_TIME_NONE;
    GstClockTime duration = GST_CLOCK_TIME_NONE;

    /* wait until we had a chance to "lock on" some payload's timestamp */
    if (G_UNLIKELY (demux->need_newsegment
            && !GST_CLOCK_TIME_IS_VALID (demux->segment_ts)))
      return GST_FLOW_OK;

    if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment) && stream->is_video
        && stream->payloads->len) {
      payload = &g_array_index (stream->payloads, AsfPayload, stream->kf_pos);
    } else {
      payload = &g_array_index (stream->payloads, AsfPayload, 0);
    }

    /* do we need to send a newsegment event */
    if ((G_UNLIKELY (demux->need_newsegment))) {
      GstEvent *segment_event;

      /* safe default if insufficient upstream info */
      if (!GST_CLOCK_TIME_IS_VALID (demux->in_gap))
        demux->in_gap = 0;

      if (demux->segment.stop == GST_CLOCK_TIME_NONE &&
          demux->segment.duration > 0) {
        /* slight HACK; prevent clipping of last bit */
        demux->segment.stop = demux->segment.duration + demux->in_gap;
      }

      /* FIXME : only if ACCURATE ! */
      if (G_LIKELY (!demux->keyunit_sync && !demux->accurate
              && (GST_CLOCK_TIME_IS_VALID (payload->ts)))
          && !GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)) {
        GST_DEBUG ("Adjusting newsegment start to %" GST_TIME_FORMAT,
            GST_TIME_ARGS (payload->ts));
        demux->segment.start = payload->ts;
        demux->segment.time = payload->ts;
      }

      GST_DEBUG_OBJECT (demux, "sending new-segment event %" GST_SEGMENT_FORMAT,
          &demux->segment);

      /* note: we fix up all timestamps to start from 0, so this should be ok */
      segment_event = gst_event_new_segment (&demux->segment);
      if (demux->segment_seqnum)
        gst_event_set_seqnum (segment_event, demux->segment_seqnum);
      gst_asf_demux_send_event_unlocked (demux, segment_event);

      /* now post any global tags we may have found */
      if (demux->taglist == NULL) {
        demux->taglist = gst_tag_list_new_empty ();
        gst_tag_list_set_scope (demux->taglist, GST_TAG_SCOPE_GLOBAL);
      }

      gst_tag_list_add (demux->taglist, GST_TAG_MERGE_REPLACE,
          GST_TAG_CONTAINER_FORMAT, "ASF", NULL);

      GST_DEBUG_OBJECT (demux, "global tags: %" GST_PTR_FORMAT, demux->taglist);
      gst_asf_demux_send_event_unlocked (demux,
          gst_event_new_tag (demux->taglist));
      demux->taglist = NULL;

      demux->need_newsegment = FALSE;
      demux->segment_seqnum = 0;
      demux->segment_running = TRUE;
    }

    /* Do we have tags pending for this stream? */
    if (G_UNLIKELY (stream->pending_tags)) {
      GST_LOG_OBJECT (stream->pad, "%" GST_PTR_FORMAT, stream->pending_tags);
      gst_pad_push_event (stream->pad,
          gst_event_new_tag (stream->pending_tags));
      stream->pending_tags = NULL;
    }

    /* We have the whole packet now so we should push the packet to
     * the src pad now. First though we should check if we need to do
     * descrambling */
    if (G_UNLIKELY (stream->span > 1)) {
      gst_asf_demux_descramble_buffer (demux, stream, &payload->buf);
    }

    payload->buf = gst_buffer_make_writable (payload->buf);

    if (G_LIKELY (!payload->keyframe)) {
      GST_BUFFER_FLAG_SET (payload->buf, GST_BUFFER_FLAG_DELTA_UNIT);
    }

    if (G_UNLIKELY (stream->discont)) {
      GST_DEBUG_OBJECT (stream->pad, "marking DISCONT on stream");
      GST_BUFFER_FLAG_SET (payload->buf, GST_BUFFER_FLAG_DISCONT);
      stream->discont = FALSE;
    }

    if (G_UNLIKELY (stream->is_video && payload->par_x && payload->par_y &&
            (payload->par_x != stream->par_x) &&
            (payload->par_y != stream->par_y))) {
      GST_DEBUG ("Updating PAR (%d/%d => %d/%d)",
          stream->par_x, stream->par_y, payload->par_x, payload->par_y);
      stream->par_x = payload->par_x;
      stream->par_y = payload->par_y;
      stream->caps = gst_caps_make_writable (stream->caps);
      gst_caps_set_simple (stream->caps, "pixel-aspect-ratio",
          GST_TYPE_FRACTION, stream->par_x, stream->par_y, NULL);
      gst_pad_set_caps (stream->pad, stream->caps);
    }

    if (G_UNLIKELY (stream->interlaced != payload->interlaced)) {
      GST_DEBUG ("Updating interlaced status (%d => %d)", stream->interlaced,
          payload->interlaced);
      stream->interlaced = payload->interlaced;
      stream->caps = gst_caps_make_writable (stream->caps);
      gst_caps_set_simple (stream->caps, "interlace-mode", G_TYPE_BOOLEAN,
          (stream->interlaced ? "mixed" : "progressive"), NULL);
      gst_pad_set_caps (stream->pad, stream->caps);
    }

    /* (sort of) interpolate timestamps using upstream "frame of reference",
     * typically useful for live src, but might (unavoidably) mess with
     * position reporting if a live src is playing not so live content
     * (e.g. rtspsrc taking some time to fall back to tcp) */
    timestamp = payload->ts;
    if (GST_CLOCK_TIME_IS_VALID (timestamp)
        && !GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)) {
      timestamp += demux->in_gap;

      /* Check if we're after the segment already, if so no need to push
       * anything here */
      if (demux->segment.stop != -1 && timestamp > demux->segment.stop) {
        GST_DEBUG_OBJECT (stream->pad,
            "Payload after segment stop %" GST_TIME_FORMAT,
            GST_TIME_ARGS (demux->segment.stop));
        ret =
            gst_flow_combiner_update_pad_flow (demux->flowcombiner, stream->pad,
            GST_FLOW_EOS);
        gst_buffer_unref (payload->buf);
        payload->buf = NULL;
        g_array_remove_index (stream->payloads, 0);
        /* Break out as soon as we have an issue */
        if (G_UNLIKELY (ret != GST_FLOW_OK))
          break;

        continue;
      }
    }

    GST_BUFFER_PTS (payload->buf) = timestamp;

    if (payload->duration == GST_CLOCK_TIME_NONE
        && stream->ext_props.avg_time_per_frame != 0) {
      duration = stream->ext_props.avg_time_per_frame * 100;
    } else {
      duration = payload->duration;
    }
    GST_BUFFER_DURATION (payload->buf) = duration;

    /* FIXME: we should really set durations on buffers if we can */

    GST_LOG_OBJECT (stream->pad, "pushing buffer, %" GST_PTR_FORMAT,
        payload->buf);

    if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment) && stream->is_video) {
      if (stream->reverse_kf_ready == TRUE && stream->kf_pos == 0) {
        GST_BUFFER_FLAG_SET (payload->buf, GST_BUFFER_FLAG_DISCONT);
      }
    } else if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)) {
      GST_BUFFER_FLAG_SET (payload->buf, GST_BUFFER_FLAG_DISCONT);
    }


    if (stream->active) {
      if (G_UNLIKELY (stream->first_buffer)) {
        if (stream->streamheader != NULL) {
          GST_DEBUG_OBJECT (stream->pad,
              "Pushing streamheader before first buffer");
          gst_pad_push (stream->pad, gst_buffer_ref (stream->streamheader));
        }
        stream->first_buffer = FALSE;
      }

      if (GST_CLOCK_TIME_IS_VALID (timestamp)
          && timestamp > demux->segment.position) {
        demux->segment.position = timestamp;
        if (GST_CLOCK_TIME_IS_VALID (duration))
          demux->segment.position += timestamp;
      }

      ret = gst_pad_push (stream->pad, payload->buf);
      ret =
          gst_flow_combiner_update_pad_flow (demux->flowcombiner, stream->pad,
          ret);
    } else {
      gst_buffer_unref (payload->buf);
      ret = GST_FLOW_OK;
    }
    payload->buf = NULL;
    if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment) && stream->is_video
        && stream->reverse_kf_ready) {
      g_array_remove_index (stream->payloads, stream->kf_pos);
      stream->kf_pos--;

      if (stream->reverse_kf_ready == TRUE && stream->kf_pos < 0) {
        stream->kf_pos = 0;
        stream->reverse_kf_ready = FALSE;
      }
    } else {
      g_array_remove_index (stream->payloads, 0);
    }

    /* Break out as soon as we have an issue */
    if (G_UNLIKELY (ret != GST_FLOW_OK))
      break;
  }

  return ret;
}
