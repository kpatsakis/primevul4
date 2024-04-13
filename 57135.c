int vp78_decode_init(AVCodecContext *avctx, int is_vp7)
{
    VP8Context *s = avctx->priv_data;
    int ret;

    s->avctx = avctx;
    s->vp7   = avctx->codec->id == AV_CODEC_ID_VP7;
    avctx->pix_fmt = AV_PIX_FMT_YUV420P;
    avctx->internal->allocate_progress = 1;

    ff_videodsp_init(&s->vdsp, 8);

    ff_vp78dsp_init(&s->vp8dsp);
    if (CONFIG_VP7_DECODER && is_vp7) {
        ff_h264_pred_init(&s->hpc, AV_CODEC_ID_VP7, 8, 1);
        ff_vp7dsp_init(&s->vp8dsp);
        s->decode_mb_row_no_filter = vp7_decode_mb_row_no_filter;
        s->filter_mb_row           = vp7_filter_mb_row;
    } else if (CONFIG_VP8_DECODER && !is_vp7) {
        ff_h264_pred_init(&s->hpc, AV_CODEC_ID_VP8, 8, 1);
        ff_vp8dsp_init(&s->vp8dsp);
        s->decode_mb_row_no_filter = vp8_decode_mb_row_no_filter;
        s->filter_mb_row           = vp8_filter_mb_row;
    }

    /* does not change for VP8 */
    memcpy(s->prob[0].scan, ff_zigzag_scan, sizeof(s->prob[0].scan));

    if ((ret = vp8_init_frames(s)) < 0) {
        ff_vp8_decode_free(avctx);
        return ret;
    }

    return 0;
}
