void avcodec_set_dimensions(AVCodecContext *s, int width, int height)
{
    int ret = ff_set_dimensions(s, width, height);
    if (ret < 0) {
        av_log(s, AV_LOG_WARNING, "Failed to set dimensions %d %d\n", width, height);
    }
}
