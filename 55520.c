static int mov_read_st3d(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    enum AVStereo3DType type;
    int mode;

    if (c->fc->nb_streams < 1)
        return 0;

    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    if (atom.size < 5) {
        av_log(c->fc, AV_LOG_ERROR, "Empty stereoscopic video box\n");
        return AVERROR_INVALIDDATA;
    }
    avio_skip(pb, 4); /* version + flags */

    mode = avio_r8(pb);
    switch (mode) {
    case 0:
        type = AV_STEREO3D_2D;
        break;
    case 1:
        type = AV_STEREO3D_TOPBOTTOM;
        break;
    case 2:
        type = AV_STEREO3D_SIDEBYSIDE;
        break;
    default:
        av_log(c->fc, AV_LOG_WARNING, "Unknown st3d mode value %d\n", mode);
        return 0;
    }

    sc->stereo3d = av_stereo3d_alloc();
    if (!sc->stereo3d)
        return AVERROR(ENOMEM);

    sc->stereo3d->type = type;
    return 0;
}
