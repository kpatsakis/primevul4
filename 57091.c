static av_cold int decode_init(AVCodecContext *avctx)
{
    SCPRContext *s = avctx->priv_data;

    switch (avctx->bits_per_coded_sample) {
    case 16: avctx->pix_fmt = AV_PIX_FMT_RGB0; break;
    case 24:
    case 32: avctx->pix_fmt = AV_PIX_FMT_BGR0; break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unsupported bitdepth %i\n", avctx->bits_per_coded_sample);
        return AVERROR_INVALIDDATA;
    }

    s->get_freq = get_freq0;
    s->decode = decode0;

    s->cxshift = avctx->bits_per_coded_sample == 16 ? 0 : 2;
    s->cbits = avctx->bits_per_coded_sample == 16 ? 0x1F : 0xFF;
    s->nbx = (avctx->width + 15) / 16;
    s->nby = (avctx->height + 15) / 16;
    s->nbcount = s->nbx * s->nby;
    s->blocks = av_malloc_array(s->nbcount, sizeof(*s->blocks));
    if (!s->blocks)
        return AVERROR(ENOMEM);

    s->last_frame = av_frame_alloc();
    s->current_frame = av_frame_alloc();
    if (!s->last_frame || !s->current_frame)
        return AVERROR(ENOMEM);

    return 0;
}
