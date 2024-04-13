AVCodec *av_codec_next(const AVCodec *c)
{
    if (c)
        return c->next;
    else
        return first_avcodec;
}
