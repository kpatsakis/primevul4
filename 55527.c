static int mov_read_targa_y216(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int ret = mov_read_extradata(c, pb, atom, AV_CODEC_ID_TARGA_Y216);

    if (!ret && c->fc->nb_streams >= 1) {
        AVCodecParameters *par = c->fc->streams[c->fc->nb_streams-1]->codecpar;
        if (par->extradata_size >= 40) {
            par->height = AV_RB16(&par->extradata[36]);
            par->width  = AV_RB16(&par->extradata[38]);
        }
    }
    return ret;
}
