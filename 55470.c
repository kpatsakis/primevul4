static void mov_parse_stsd_subtitle(MOVContext *c, AVIOContext *pb,
                                    AVStream *st, MOVStreamContext *sc,
                                    int64_t size)
{
    MOVAtom fake_atom = { .size = size };
    if (st->codecpar->codec_tag != AV_RL32("mp4s"))
        mov_read_glbl(c, pb, fake_atom);
    st->codecpar->width  = sc->width;
    st->codecpar->height = sc->height;
}
