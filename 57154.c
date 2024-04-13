static av_cold int vp8_decode_init_thread_copy(AVCodecContext *avctx)
{
    VP8Context *s = avctx->priv_data;
    int ret;

    s->avctx = avctx;

    if ((ret = vp8_init_frames(s)) < 0) {
        ff_vp8_decode_free(avctx);
        return ret;
    }

    return 0;
}
