int ff_h264_handle_frag_packet(AVPacket *pkt, const uint8_t *buf, int len,
                               int start_bit, const uint8_t *nal_header,
                               int nal_header_len)
{
    int ret;
    int tot_len = len;
    int pos = 0;
    if (start_bit)
        tot_len += sizeof(start_sequence) + nal_header_len;
    if ((ret = av_new_packet(pkt, tot_len)) < 0)
        return ret;
    if (start_bit) {
        memcpy(pkt->data + pos, start_sequence, sizeof(start_sequence));
        pos += sizeof(start_sequence);
        memcpy(pkt->data + pos, nal_header, nal_header_len);
        pos += nal_header_len;
    }
    memcpy(pkt->data + pos, buf, len);
    return 0;
}
