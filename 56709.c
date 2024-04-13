int ff_rtmp_packet_read(URLContext *h, RTMPPacket *p,
                        int chunk_size, RTMPPacket **prev_pkt, int *nb_prev_pkt)
{
    uint8_t hdr;

    if (ffurl_read(h, &hdr, 1) != 1)
        return AVERROR(EIO);

    return ff_rtmp_packet_read_internal(h, p, chunk_size, prev_pkt,
                                        nb_prev_pkt, hdr);
}
