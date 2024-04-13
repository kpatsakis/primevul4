static int asf_read_file_properties(AVFormatContext *s, int64_t size)
{
    ASFContext *asf = s->priv_data;
    AVIOContext *pb = s->pb;

    ff_get_guid(pb, &asf->hdr.guid);
    asf->hdr.file_size   = avio_rl64(pb);
    asf->hdr.create_time = avio_rl64(pb);
    avio_rl64(pb);                               /* number of packets */
    asf->hdr.play_time   = avio_rl64(pb);
    asf->hdr.send_time   = avio_rl64(pb);
    asf->hdr.preroll     = avio_rl32(pb);
    asf->hdr.ignore      = avio_rl32(pb);
    asf->hdr.flags       = avio_rl32(pb);
    asf->hdr.min_pktsize = avio_rl32(pb);
    asf->hdr.max_pktsize = avio_rl32(pb);
    if (asf->hdr.min_pktsize >= (1U << 29))
        return AVERROR_INVALIDDATA;
    asf->hdr.max_bitrate = avio_rl32(pb);
    s->packet_size       = asf->hdr.max_pktsize;

    return 0;
}
