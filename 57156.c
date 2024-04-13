static void vp8_decode_mv_mb_modes(AVCodecContext *avctx, VP8Frame *cur_frame,
                                   VP8Frame *prev_frame)
{
    vp78_decode_mv_mb_modes(avctx, cur_frame, prev_frame, IS_VP8);
}
