int avcodec_is_open(AVCodecContext *s)
{
    return !!s->internal;
}
