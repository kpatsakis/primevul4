local void append_len(struct job *job, size_t len)
{
    struct space *lens;

    assert(len < 539000896UL);
    if (job->lens == NULL)
        job->lens = get_space(&lens_pool);
    lens = job->lens;
    if (lens->size < lens->len + 3)
        grow_space(lens);
    if (len < 64)
        lens->buf[lens->len++] = len + 128;
    else if (len < 32832U) {
        len -= 64;
        lens->buf[lens->len++] = len >> 8;
        lens->buf[lens->len++] = len;
    }
    else if (len < 2129984UL) {
        len -= 32832U;
        lens->buf[lens->len++] = (len >> 16) + 192;
        lens->buf[lens->len++] = len >> 8;
        lens->buf[lens->len++] = len;
    }
    else {
        len -= 2129984UL;
        lens->buf[lens->len++] = (len >> 24) + 224;
        lens->buf[lens->len++] = len >> 16;
        lens->buf[lens->len++] = len >> 8;
        lens->buf[lens->len++] = len;
    }
}
