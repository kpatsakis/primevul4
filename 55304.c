int ff_h264_handle_aggregated_packet(AVFormatContext *ctx, PayloadContext *data, AVPacket *pkt,
                                     const uint8_t *buf, int len,
                                     int skip_between, int *nal_counters,
                                     int nal_mask)
{
    int pass         = 0;
    int total_length = 0;
    uint8_t *dst     = NULL;
    int ret;

    for (pass = 0; pass < 2; pass++) {
        const uint8_t *src = buf;
        int src_len        = len;

        while (src_len > 2) {
            uint16_t nal_size = AV_RB16(src);

            src     += 2;
            src_len -= 2;

            if (nal_size <= src_len) {
                if (pass == 0) {
                    total_length += sizeof(start_sequence) + nal_size;
                } else {
                    memcpy(dst, start_sequence, sizeof(start_sequence));
                    dst += sizeof(start_sequence);
                    memcpy(dst, src, nal_size);
                    if (nal_counters)
                        nal_counters[(*src) & nal_mask]++;
                    dst += nal_size;
                }
            } else {
                av_log(ctx, AV_LOG_ERROR,
                       "nal size exceeds length: %d %d\n", nal_size, src_len);
                return AVERROR_INVALIDDATA;
            }

            src     += nal_size + skip_between;
            src_len -= nal_size + skip_between;
        }

        if (pass == 0) {
            /* now we know the total size of the packet (with the
             * start sequences added) */
            if ((ret = av_new_packet(pkt, total_length)) < 0)
                return ret;
            dst = pkt->data;
        }
    }

    return 0;
}
