static int mov_read_avid(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int ret = mov_read_extradata(c, pb, atom, AV_CODEC_ID_AVUI);
    if(ret == 0)
        ret = mov_read_extradata(c, pb, atom, AV_CODEC_ID_DNXHD);
    return ret;
}
