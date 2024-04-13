int ff_decode_frame_props(AVCodecContext *avctx, AVFrame *frame)
{
    return ff_init_buffer_info(avctx, frame);
}
