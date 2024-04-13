static int var_read_int(AVIOContext *pb, int size)
{
    int v;
    char *s = var_read_string(pb, size);
    if (!s)
        return 0;
    v = strtol(s, NULL, 10);
    av_free(s);
    return v;
}
