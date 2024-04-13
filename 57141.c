static int vp7_decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                            AVPacket *avpkt)
{
    return vp78_decode_frame(avctx, data, got_frame, avpkt, IS_VP7);
}
