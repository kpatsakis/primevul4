static av_cold int cdxl_decode_end(AVCodecContext *avctx)
{
    CDXLVideoContext *c = avctx->priv_data;

    av_freep(&c->new_video);

    return 0;
}
