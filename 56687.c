static av_cold int dnxhd_decode_init_thread_copy(AVCodecContext *avctx)
{
    DNXHDContext *ctx = avctx->priv_data;

    ctx->avctx = avctx;
    ctx->cid = -1;

    ctx->rows = av_mallocz_array(avctx->thread_count, sizeof(RowContext));
    if (!ctx->rows)
        return AVERROR(ENOMEM);

    return 0;
}
