gst_asf_demux_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstASFDemux *demux;

  demux = GST_ASF_DEMUX (parent);

  GST_LOG_OBJECT (demux,
      "buffer: size=%" G_GSIZE_FORMAT ", offset=%" G_GINT64_FORMAT ", time=%"
      GST_TIME_FORMAT, gst_buffer_get_size (buf), GST_BUFFER_OFFSET (buf),
      GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)));

  if (G_UNLIKELY (GST_BUFFER_IS_DISCONT (buf))) {
    GST_DEBUG_OBJECT (demux, "received DISCONT");
    gst_asf_demux_mark_discont (demux);
  }

  if (G_UNLIKELY ((!GST_CLOCK_TIME_IS_VALID (demux->in_gap) &&
              GST_BUFFER_TIMESTAMP_IS_VALID (buf)))) {
    demux->in_gap = GST_BUFFER_TIMESTAMP (buf) - demux->in_segment.start;
    GST_DEBUG_OBJECT (demux, "upstream segment start %" GST_TIME_FORMAT
        ", interpolation gap: %" GST_TIME_FORMAT,
        GST_TIME_ARGS (demux->in_segment.start), GST_TIME_ARGS (demux->in_gap));
  }

  gst_adapter_push (demux->adapter, buf);

  switch (demux->state) {
    case GST_ASF_DEMUX_STATE_INDEX:{
      gint result = gst_asf_demux_check_header (demux);
      if (result == GST_ASF_DEMUX_CHECK_HEADER_NEED_DATA)       /* need more data */
        break;

      if (result == GST_ASF_DEMUX_CHECK_HEADER_NO) {
        /* we don't care about this, probably an index */
        /* TODO maybe would be smarter to skip all the indices
         * until we got a new header or EOS to decide */
        GST_LOG_OBJECT (demux, "Received index object, its EOS");
        goto eos;
      } else {
        GST_INFO_OBJECT (demux, "Chained asf starting");
        /* cleanup and get ready for a chained asf */
        gst_asf_demux_reset (demux, TRUE);
        /* fall through */
      }
    }
    case GST_ASF_DEMUX_STATE_HEADER:{
      ret = gst_asf_demux_chain_headers (demux);
      if (demux->state != GST_ASF_DEMUX_STATE_DATA)
        break;
      /* otherwise fall through */
    }
    case GST_ASF_DEMUX_STATE_DATA:
    {
      guint64 data_size;

      data_size = demux->packet_size;

      while (gst_adapter_available (demux->adapter) >= data_size) {
        GstBuffer *buf;
        GstAsfDemuxParsePacketError err;

        /* we don't know the length of the stream
         * check for a chained asf everytime */
        if (demux->num_packets == 0) {
          gint result = gst_asf_demux_check_header (demux);

          if (result == GST_ASF_DEMUX_CHECK_HEADER_YES) {
            GST_INFO_OBJECT (demux, "Chained asf starting");
            /* cleanup and get ready for a chained asf */
            gst_asf_demux_reset (demux, TRUE);
            break;
          }
        } else if (G_UNLIKELY (demux->num_packets != 0 && demux->packet >= 0
                && demux->packet >= demux->num_packets)) {
          /* do not overshoot data section when streaming */
          break;
        }

        buf = gst_adapter_take_buffer (demux->adapter, data_size);

        /* FIXME: We should tally up fatal errors and error out only
         * after a few broken packets in a row? */
        err = gst_asf_demux_parse_packet (demux, buf);

        gst_buffer_unref (buf);

        if (G_LIKELY (err == GST_ASF_DEMUX_PARSE_PACKET_ERROR_NONE))
          ret = gst_asf_demux_push_complete_payloads (demux, FALSE);
        else
          GST_WARNING_OBJECT (demux, "Parse error");

        if (demux->packet >= 0)
          ++demux->packet;
      }
      if (G_UNLIKELY (demux->num_packets != 0 && demux->packet >= 0
              && demux->packet >= demux->num_packets)) {
        demux->state = GST_ASF_DEMUX_STATE_INDEX;
      }
      break;
    }
    default:
      g_assert_not_reached ();
  }

done:
  if (ret != GST_FLOW_OK)
    GST_DEBUG_OBJECT (demux, "flow: %s", gst_flow_get_name (ret));

  return ret;

eos:
  {
    GST_DEBUG_OBJECT (demux, "Handled last packet, setting EOS");
    ret = GST_FLOW_EOS;
    goto done;
  }
}
