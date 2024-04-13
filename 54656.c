static av_cold int smka_decode_init(AVCodecContext *avctx)
{
    if (avctx->channels < 1 || avctx->channels > 2) {
        av_log(avctx, AV_LOG_ERROR, "invalid number of channels\n");
        return AVERROR_INVALIDDATA;
    }
    avctx->channel_layout = (avctx->channels==2) ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;
    avctx->sample_fmt = avctx->bits_per_coded_sample == 8 ? AV_SAMPLE_FMT_U8 : AV_SAMPLE_FMT_S16;

    return 0;
}
