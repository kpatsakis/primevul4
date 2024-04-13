static av_cold int cdxl_decode_init(AVCodecContext *avctx)
{
    CDXLVideoContext *c = avctx->priv_data;

    c->new_video_size = 0;
    c->avctx          = avctx;

    return 0;
}
