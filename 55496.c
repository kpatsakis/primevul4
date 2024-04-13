static int mov_read_dvc1(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    uint8_t profile_level;
    int ret;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if (atom.size >= (1<<28) || atom.size < 7)
        return AVERROR_INVALIDDATA;

    profile_level = avio_r8(pb);
    if ((profile_level & 0xf0) != 0xc0)
        return 0;

    avio_seek(pb, 6, SEEK_CUR);
    av_freep(&st->codecpar->extradata);
    ret = ff_get_extradata(c->fc, st->codecpar, pb, atom.size - 7);
    if (ret < 0)
        return ret;

    return 0;
}
