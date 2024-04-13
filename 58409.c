int ff_reget_buffer(AVCodecContext *avctx, AVFrame *frame)
{
    int ret = reget_buffer_internal(avctx, frame);
    if (ret < 0)
        av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");
    return ret;
}
