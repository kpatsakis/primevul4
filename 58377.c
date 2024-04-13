AVCodecParameters *avcodec_parameters_alloc(void)
{
    AVCodecParameters *par = av_mallocz(sizeof(*par));

    if (!par)
        return NULL;
    codec_parameters_reset(par);
    return par;
}
