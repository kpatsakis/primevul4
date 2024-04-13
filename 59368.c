gst_asf_demux_loop (GstASFDemux * demux)
{
  GstFlowReturn flow = GST_FLOW_OK;
  GstBuffer *buf = NULL;
  guint64 off;

  if (G_UNLIKELY (demux->state == GST_ASF_DEMUX_STATE_HEADER)) {
    if (!gst_asf_demux_pull_headers (demux, &flow)) {
      goto pause;
    }

    flow = gst_asf_demux_pull_indices (demux);
    if (flow != GST_FLOW_OK)
      goto pause;
  }

  g_assert (demux->state == GST_ASF_DEMUX_STATE_DATA);

  if (G_UNLIKELY (demux->num_packets != 0
          && demux->packet >= demux->num_packets))
    goto eos;

  GST_LOG_OBJECT (demux, "packet %u/%u", (guint) demux->packet + 1,
      (guint) demux->num_packets);

  off = demux->data_offset + (demux->packet * demux->packet_size);

  if (G_UNLIKELY (!gst_asf_demux_pull_data (demux, off,
              demux->packet_size * demux->speed_packets, &buf, &flow))) {
    GST_DEBUG_OBJECT (demux, "got flow %s", gst_flow_get_name (flow));
    if (flow == GST_FLOW_EOS) {
      goto eos;
    } else if (flow == GST_FLOW_FLUSHING) {
      GST_DEBUG_OBJECT (demux, "Not fatal");
      goto pause;
    } else {
      goto read_failed;
    }
  }

  if (G_LIKELY (demux->speed_packets == 1)) {
    GstAsfDemuxParsePacketError err;
    err = gst_asf_demux_parse_packet (demux, buf);
    if (G_UNLIKELY (err != GST_ASF_DEMUX_PARSE_PACKET_ERROR_NONE)) {
      /* when we don't know when the data object ends, we should check
       * for a chained asf */
      if (demux->num_packets == 0) {
        if (gst_asf_demux_check_buffer_is_header (demux, buf)) {
          GST_INFO_OBJECT (demux, "Chained asf found");
          demux->base_offset = off;
          gst_asf_demux_reset (demux, TRUE);
          gst_buffer_unref (buf);
          return;
        }
      }
      /* FIXME: We should tally up fatal errors and error out only
       * after a few broken packets in a row? */

      GST_INFO_OBJECT (demux, "Ignoring recoverable parse error");
      gst_buffer_unref (buf);

      if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)
          && !demux->seek_to_cur_pos) {
        --demux->packet;
        if (demux->packet < 0) {
          goto eos;
        }
      } else {
        ++demux->packet;
      }

      return;
    }

    flow = gst_asf_demux_push_complete_payloads (demux, FALSE);

    if (GST_ASF_DEMUX_IS_REVERSE_PLAYBACK (demux->segment)
        && !demux->seek_to_cur_pos) {
      --demux->packet;
      if (demux->packet < 0) {
        goto eos;
      }
    } else {
      ++demux->packet;
    }

  } else {
    guint n;
    for (n = 0; n < demux->speed_packets; n++) {
      GstBuffer *sub;
      GstAsfDemuxParsePacketError err;

      sub =
          gst_buffer_copy_region (buf, GST_BUFFER_COPY_ALL,
          n * demux->packet_size, demux->packet_size);
      err = gst_asf_demux_parse_packet (demux, sub);
      if (G_UNLIKELY (err != GST_ASF_DEMUX_PARSE_PACKET_ERROR_NONE)) {
        /* when we don't know when the data object ends, we should check
         * for a chained asf */
        if (demux->num_packets == 0) {
          if (gst_asf_demux_check_buffer_is_header (demux, sub)) {
            GST_INFO_OBJECT (demux, "Chained asf found");
            demux->base_offset = off + n * demux->packet_size;
            gst_asf_demux_reset (demux, TRUE);
            gst_buffer_unref (sub);
            gst_buffer_unref (buf);
            return;
          }
        }
        /* FIXME: We should tally up fatal errors and error out only
         * after a few broken packets in a row? */

        GST_INFO_OBJECT (demux, "Ignoring recoverable parse error");
        flow = GST_FLOW_OK;
      }

      gst_buffer_unref (sub);

      if (err == GST_ASF_DEMUX_PARSE_PACKET_ERROR_NONE)
        flow = gst_asf_demux_push_complete_payloads (demux, FALSE);

      ++demux->packet;

    }

    /* reset speed pull */
    demux->speed_packets = 1;
  }

  gst_buffer_unref (buf);

  if (G_UNLIKELY ((demux->num_packets > 0
              && demux->packet >= demux->num_packets)
          || flow == GST_FLOW_EOS)) {
    GST_LOG_OBJECT (demux, "reached EOS");
    goto eos;
  }

  if (G_UNLIKELY (flow != GST_FLOW_OK)) {
    GST_DEBUG_OBJECT (demux, "pushing complete payloads failed");
    goto pause;
  }

  /* check if we're at the end of the configured segment */
  /* FIXME: check if segment end reached etc. */

  return;

eos:
  {
    /* if we haven't activated our streams yet, this might be because we have
     * less data queued than required for preroll; force stream activation and
     * send any pending payloads before sending EOS */
    if (!demux->activated_streams)
      flow = gst_asf_demux_push_complete_payloads (demux, TRUE);

    /* we want to push an eos or post a segment-done in any case */
    if (demux->segment.flags & GST_SEEK_FLAG_SEGMENT) {
      gint64 stop;

      /* for segment playback we need to post when (in stream time)
       * we stopped, this is either stop (when set) or the duration. */
      if ((stop = demux->segment.stop) == -1)
        stop = demux->segment.duration;

      GST_INFO_OBJECT (demux, "Posting segment-done, at end of segment");
      gst_element_post_message (GST_ELEMENT_CAST (demux),
          gst_message_new_segment_done (GST_OBJECT (demux), GST_FORMAT_TIME,
              stop));
      gst_asf_demux_send_event_unlocked (demux,
          gst_event_new_segment_done (GST_FORMAT_TIME, stop));
    } else if (flow != GST_FLOW_EOS) {
      /* check if we have a chained asf, in case, we don't eos yet */
      if (gst_asf_demux_check_chained_asf (demux)) {
        GST_INFO_OBJECT (demux, "Chained ASF starting");
        gst_asf_demux_reset (demux, TRUE);
        return;
      }
    }

    if (!(demux->segment.flags & GST_SEEK_FLAG_SEGMENT)) {
      if (demux->activated_streams) {
        /* normal playback, send EOS to all linked pads */
        GST_INFO_OBJECT (demux, "Sending EOS, at end of stream");
        gst_asf_demux_send_event_unlocked (demux, gst_event_new_eos ());
      } else {
        GST_WARNING_OBJECT (demux, "EOS without exposed streams");
        flow = GST_FLOW_EOS;
      }
    }
    /* ... and fall through to pause */
  }
pause:
  {
    GST_DEBUG_OBJECT (demux, "pausing task, flow return: %s",
        gst_flow_get_name (flow));
    demux->segment_running = FALSE;
    gst_pad_pause_task (demux->sinkpad);

    /* For the error cases */
    if (flow == GST_FLOW_EOS && !demux->activated_streams) {
      GST_ELEMENT_ERROR (demux, STREAM, WRONG_TYPE, (NULL),
          ("This doesn't seem to be an ASF file"));
    } else if (flow < GST_FLOW_EOS || flow == GST_FLOW_NOT_LINKED) {
      /* Post an error. Hopefully something else already has, but if not... */
      GST_ELEMENT_FLOW_ERROR (demux, flow);
      gst_asf_demux_send_event_unlocked (demux, gst_event_new_eos ());
    }

    return;
  }

/* ERRORS */
read_failed:
  {
    GST_DEBUG_OBJECT (demux, "Read failed, doh");
    flow = GST_FLOW_EOS;
    goto pause;
  }
#if 0
  /* See FIXMEs above */
parse_error:
  {
    gst_buffer_unref (buf);
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("Error parsing ASF packet %u", (guint) demux->packet));
    gst_asf_demux_send_event_unlocked (demux, gst_event_new_eos ());
    flow = GST_FLOW_ERROR;
    goto pause;
  }
#endif
}
