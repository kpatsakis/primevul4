int ff_rtmp_packet_read_internal(URLContext *h, RTMPPacket *p, int chunk_size,
                                 RTMPPacket **prev_pkt, int *nb_prev_pkt,
                                 uint8_t hdr)
{
    while (1) {
        int ret = rtmp_packet_read_one_chunk(h, p, chunk_size, prev_pkt,
                                             nb_prev_pkt, hdr);
        if (ret > 0 || ret != AVERROR(EAGAIN))
            return ret;

        if (ffurl_read(h, &hdr, 1) != 1)
            return AVERROR(EIO);
    }
}
