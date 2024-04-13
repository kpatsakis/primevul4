static av_cold int decode_close(AVCodecContext *avctx)
{
    SCPRContext *s = avctx->priv_data;

    av_freep(&s->blocks);
    av_frame_free(&s->last_frame);
    av_frame_free(&s->current_frame);

    return 0;
}
