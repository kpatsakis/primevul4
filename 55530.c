static int mov_read_tmcd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;

    if (c->fc->nb_streams < 1)
        return AVERROR_INVALIDDATA;
    sc = c->fc->streams[c->fc->nb_streams - 1]->priv_data;
    sc->timecode_track = avio_rb32(pb);
    return 0;
}
