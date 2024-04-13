static int mov_read_free(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int ret;
    uint8_t content[16];

    if (atom.size < 8)
        return 0;

    ret = avio_read(pb, content, FFMIN(sizeof(content), atom.size));
    if (ret < 0)
        return ret;

    if (   !c->found_moov
        && !c->found_mdat
        && !memcmp(content, "Anevia\x1A\x1A", 8)
        && c->use_mfra_for == FF_MOV_FLAG_MFRA_AUTO) {
        c->use_mfra_for = FF_MOV_FLAG_MFRA_PTS;
    }

    return 0;
}
