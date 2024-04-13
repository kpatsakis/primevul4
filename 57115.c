av_cold int ff_vp8_decode_init(AVCodecContext *avctx)
{
    return vp78_decode_init(avctx, IS_VP8);
}
