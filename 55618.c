static int mxf_get_d10_aes3_packet(AVIOContext *pb, AVStream *st, AVPacket *pkt, int64_t length)
{
    const uint8_t *buf_ptr, *end_ptr;
    uint8_t *data_ptr;
    int i;

    if (length > 61444) /* worst case PAL 1920 samples 8 channels */
        return AVERROR_INVALIDDATA;
    length = av_get_packet(pb, pkt, length);
    if (length < 0)
        return length;
    data_ptr = pkt->data;
    end_ptr = pkt->data + length;
    buf_ptr = pkt->data + 4; /* skip SMPTE 331M header */
    for (; end_ptr - buf_ptr >= st->codecpar->channels * 4; ) {
        for (i = 0; i < st->codecpar->channels; i++) {
            uint32_t sample = bytestream_get_le32(&buf_ptr);
            if (st->codecpar->bits_per_coded_sample == 24)
                bytestream_put_le24(&data_ptr, (sample >> 4) & 0xffffff);
            else
                bytestream_put_le16(&data_ptr, (sample >> 12) & 0xffff);
        }
        buf_ptr += 32 - st->codecpar->channels*4; // always 8 channels stored SMPTE 331M
    }
    av_shrink_packet(pkt, data_ptr - pkt->data);
    return 0;
}
