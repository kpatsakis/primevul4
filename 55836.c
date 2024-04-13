static AVRational var_read_float(AVIOContext *pb, int size)
{
    AVRational v;
    char *s = var_read_string(pb, size);
    if (!s)
        return (AVRational) { 0, 0 };
    v = av_d2q(av_strtod(s, NULL), INT_MAX);
    av_free(s);
    return v;
}
