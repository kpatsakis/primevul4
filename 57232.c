static av_cold int dfa_decode_end(AVCodecContext *avctx)
{
    DfaContext *s = avctx->priv_data;

    av_freep(&s->frame_buf);

    return 0;
}
