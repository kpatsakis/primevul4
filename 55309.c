static int h264_handle_packet(AVFormatContext *ctx, PayloadContext *data,
                              AVStream *st, AVPacket *pkt, uint32_t *timestamp,
                              const uint8_t *buf, int len, uint16_t seq,
                              int flags)
{
    uint8_t nal;
    uint8_t type;
    int result = 0;

    if (!len) {
        av_log(ctx, AV_LOG_ERROR, "Empty H.264 RTP packet\n");
        return AVERROR_INVALIDDATA;
    }
    nal  = buf[0];
    type = nal & 0x1f;

    /* Simplify the case (these are all the NAL types used internally by
     * the H.264 codec). */
    if (type >= 1 && type <= 23)
        type = 1;
    switch (type) {
    case 0:                    // undefined, but pass them through
    case 1:
        if ((result = av_new_packet(pkt, len + sizeof(start_sequence))) < 0)
            return result;
        memcpy(pkt->data, start_sequence, sizeof(start_sequence));
        memcpy(pkt->data + sizeof(start_sequence), buf, len);
        COUNT_NAL_TYPE(data, nal);
        break;

    case 24:                   // STAP-A (one packet, multiple nals)
        buf++;
        len--;
        result = ff_h264_handle_aggregated_packet(ctx, data, pkt, buf, len, 0,
                                                  NAL_COUNTERS, NAL_MASK);
        break;

    case 25:                   // STAP-B
    case 26:                   // MTAP-16
    case 27:                   // MTAP-24
    case 29:                   // FU-B
        avpriv_report_missing_feature(ctx, "RTP H.264 NAL unit type %d", type);
        result = AVERROR_PATCHWELCOME;
        break;

    case 28:                   // FU-A (fragmented nal)
        result = h264_handle_packet_fu_a(ctx, data, pkt, buf, len,
                                         NAL_COUNTERS, NAL_MASK);
        break;

    case 30:                   // undefined
    case 31:                   // undefined
    default:
        av_log(ctx, AV_LOG_ERROR, "Undefined type (%d)\n", type);
        result = AVERROR_INVALIDDATA;
        break;
    }

    pkt->stream_index = st->index;

    return result;
}
