int avcodec_parameters_copy(AVCodecParameters *dst, const AVCodecParameters *src)
{
    codec_parameters_reset(dst);
    memcpy(dst, src, sizeof(*dst));

    dst->extradata      = NULL;
    dst->extradata_size = 0;
    if (src->extradata) {
        dst->extradata = av_mallocz(src->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!dst->extradata)
            return AVERROR(ENOMEM);
        memcpy(dst->extradata, src->extradata, src->extradata_size);
        dst->extradata_size = src->extradata_size;
    }

    return 0;
}
