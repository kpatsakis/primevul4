static inline void get_strl(AVIOContext *pb, char *buf, int buf_size, int len)
{
    int i;
    char *q, r;

    q = buf;
    for(i=0;i<len;i++) {
        r = avio_r8(pb);
        if (i < buf_size - 1)
            *q++ = r;
    }
    if (buf_size > 0) *q = '\0';
}
