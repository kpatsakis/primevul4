static int asf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    ASFContext *asf = s->priv_data;

    for (;;) {
        int ret;

        /* parse cached packets, if any */
        if ((ret = asf_parse_packet(s, s->pb, pkt)) <= 0)
            return ret;
        if ((ret = asf_get_packet(s, s->pb)) < 0)
            assert(asf->packet_size_left < FRAME_HEADER_SIZE ||
                   asf->packet_segments < 1);
        asf->packet_time_start = 0;
    }
}
