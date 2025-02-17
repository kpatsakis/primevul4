static char *var_read_string(AVIOContext *pb, int size)
{
    int n;
    char *str;

    if (size < 0 || size == INT_MAX)
        return NULL;

    str = av_malloc(size + 1);
    if (!str)
        return NULL;
    n = avio_get_str(pb, size, str, size + 1);
    if (n < size)
        avio_skip(pb, size - n);
    return str;
}
