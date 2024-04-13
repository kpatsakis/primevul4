static av_cold int ape_decode_close(AVCodecContext *avctx)
{
    APEContext *s = avctx->priv_data;
    int i;

    for (i = 0; i < APE_FILTER_LEVELS; i++)
        av_freep(&s->filterbuf[i]);

    av_freep(&s->decoded_buffer);
    av_freep(&s->data);
    s->decoded_size = s->data_size = 0;

    return 0;
}
