static int mov_read_coll(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;
    int version;

    if (c->fc->nb_streams < 1)
        return AVERROR_INVALIDDATA;

    sc = c->fc->streams[c->fc->nb_streams - 1]->priv_data;

    if (atom.size < 5) {
        av_log(c->fc, AV_LOG_ERROR, "Empty Content Light Level box\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb);
    if (version) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported Content Light Level box version %d\n", version);
        return 0;
    }
    avio_skip(pb, 3); /* flags */

    sc->coll = av_content_light_metadata_alloc(&sc->coll_size);
    if (!sc->coll)
        return AVERROR(ENOMEM);

    sc->coll->MaxCLL  = avio_rb16(pb);
    sc->coll->MaxFALL = avio_rb16(pb);

    return 0;
}
