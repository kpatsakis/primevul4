static av_cold int dfa_decode_init(AVCodecContext *avctx)
{
    DfaContext *s = avctx->priv_data;

    avctx->pix_fmt = AV_PIX_FMT_PAL8;

    if (!avctx->width || !avctx->height)
        return AVERROR_INVALIDDATA;

    av_assert0(av_image_check_size(avctx->width, avctx->height, 0, avctx) >= 0);

    s->frame_buf = av_mallocz(avctx->width * avctx->height);
    if (!s->frame_buf)
        return AVERROR(ENOMEM);

    return 0;
}
