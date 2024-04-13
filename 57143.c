static int vp7_decode_init(AVCodecContext *avctx)
{
    return vp78_decode_init(avctx, IS_VP7);
}
