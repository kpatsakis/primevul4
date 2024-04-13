gst_asf_demux_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstASFDemux *demux;
  gboolean ret = TRUE;

  demux = GST_ASF_DEMUX (parent);

  GST_LOG_OBJECT (demux, "handling %s event", GST_EVENT_TYPE_NAME (event));
  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEGMENT:{
      const GstSegment *segment;

      gst_event_parse_segment (event, &segment);

      if (segment->format == GST_FORMAT_BYTES) {
        if (demux->packet_size && segment->start > demux->data_offset)
          demux->packet = (segment->start - demux->data_offset) /
              demux->packet_size;
        else
          demux->packet = 0;
      } else if (segment->format == GST_FORMAT_TIME) {
        /* do not know packet position, not really a problem */
        demux->packet = -1;
      } else {
        GST_WARNING_OBJECT (demux, "unsupported newsegment format, ignoring");
        gst_event_unref (event);
        break;
      }

      /* record upstream segment for interpolation */
      if (segment->format != demux->in_segment.format)
        gst_segment_init (&demux->in_segment, GST_FORMAT_UNDEFINED);
      gst_segment_copy_into (segment, &demux->in_segment);

      /* in either case, clear some state and generate newsegment later on */
      GST_OBJECT_LOCK (demux);
      demux->segment_ts = GST_CLOCK_TIME_NONE;
      demux->in_gap = GST_CLOCK_TIME_NONE;
      demux->need_newsegment = TRUE;
      demux->segment_seqnum = gst_event_get_seqnum (event);
      gst_asf_demux_reset_stream_state_after_discont (demux);
      /* if we seek back after reaching EOS, go back to packet reading state */
      if (demux->data_offset > 0 && segment->start >= demux->data_offset
          && demux->state == GST_ASF_DEMUX_STATE_INDEX) {
        demux->state = GST_ASF_DEMUX_STATE_DATA;
      }
      GST_OBJECT_UNLOCK (demux);

      gst_event_unref (event);
      break;
    }
    case GST_EVENT_EOS:{
      GstFlowReturn flow;

      if (demux->state == GST_ASF_DEMUX_STATE_HEADER) {
        GST_ELEMENT_ERROR (demux, STREAM, DEMUX,
            (_("This stream contains no data.")),
            ("got eos and didn't receive a complete header object"));
        break;
      }
      flow = gst_asf_demux_push_complete_payloads (demux, TRUE);
      if (!demux->activated_streams) {
        /* If we still haven't got activated streams, the file is most likely corrupt */
        GST_ELEMENT_ERROR (demux, STREAM, WRONG_TYPE,
            (_("This stream contains no data.")),
            ("got eos and didn't receive a complete header object"));
        break;
      }
      if (flow < GST_FLOW_EOS || flow == GST_FLOW_NOT_LINKED) {
        GST_ELEMENT_FLOW_ERROR (demux, flow);
        break;
      }

      GST_OBJECT_LOCK (demux);
      gst_adapter_clear (demux->adapter);
      GST_OBJECT_UNLOCK (demux);
      gst_asf_demux_send_event_unlocked (demux, event);
      break;
    }

    case GST_EVENT_FLUSH_STOP:
      GST_OBJECT_LOCK (demux);
      gst_asf_demux_reset_stream_state_after_discont (demux);
      GST_OBJECT_UNLOCK (demux);
      gst_asf_demux_send_event_unlocked (demux, event);
      /* upon activation, latency is no longer introduced, e.g. after seek */
      if (demux->activated_streams)
        demux->latency = 0;
      break;

    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }

  return ret;
}
