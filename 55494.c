static int mov_read_dpxe(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    return mov_read_extradata(c, pb, atom, AV_CODEC_ID_R10K);
}
