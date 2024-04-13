static int asf_read_close(AVFormatContext *s)
{
    asf_reset_header(s);

    return 0;
}
