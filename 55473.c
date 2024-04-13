static int mov_read_aclr(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int ret = 0;
    int length = 0;
    uint64_t original_size;
    if (c->fc->nb_streams >= 1) {
        AVCodecParameters *par = c->fc->streams[c->fc->nb_streams-1]->codecpar;
        if (par->codec_id == AV_CODEC_ID_H264)
            return 0;
        if (atom.size == 16) {
            original_size = par->extradata_size;
            ret = mov_realloc_extradata(par, atom);
            if (!ret) {
                length =  mov_read_atom_into_extradata(c, pb, atom, par, par->extradata + original_size);
                if (length == atom.size) {
                    const uint8_t range_value = par->extradata[original_size + 19];
                    switch (range_value) {
                    case 1:
                        par->color_range = AVCOL_RANGE_MPEG;
                        break;
                    case 2:
                        par->color_range = AVCOL_RANGE_JPEG;
                        break;
                    default:
                        av_log(c, AV_LOG_WARNING, "ignored unknown aclr value (%d)\n", range_value);
                        break;
                    }
                    ff_dlog(c, "color_range: %d\n", par->color_range);
                } else {
                  /* For some reason the whole atom was not added to the extradata */
                  av_log(c, AV_LOG_ERROR, "aclr not decoded - incomplete atom\n");
                }
            } else {
                av_log(c, AV_LOG_ERROR, "aclr not decoded - unable to add atom to extradata\n");
            }
        } else {
            av_log(c, AV_LOG_WARNING, "aclr not decoded - unexpected size %"PRId64"\n", atom.size);
        }
    }

    return ret;
}
