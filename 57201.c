static av_cold int webp_decode_close(AVCodecContext *avctx)
{
    WebPContext *s = avctx->priv_data;

    if (s->initialized)
        return ff_vp8_decode_free(avctx);

    return 0;
}
