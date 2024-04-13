static uint8_t *iso88591_to_utf8(const uint8_t *in, size_t size_in)
{
    size_t extra = 0, i;
    uint8_t *out, *q;

    for (i = 0; i < size_in; i++)
        extra += in[i] >= 0x80;
    if (size_in == SIZE_MAX || extra > SIZE_MAX - size_in - 1)
        return NULL;
    q = out = av_malloc(size_in + extra + 1);
    if (!out)
        return NULL;
    for (i = 0; i < size_in; i++) {
        if (in[i] >= 0x80) {
            *(q++) = 0xC0 | (in[i] >> 6);
            *(q++) = 0x80 | (in[i] & 0x3F);
        } else {
            *(q++) = in[i];
        }
    }
    *(q++) = 0;
    return out;
}
