static int ascii2index(const uint8_t *cpixel, int cpp)
{
    const uint8_t *p = cpixel;
    int n = 0, m = 1, i;

    for (i = 0; i < cpp; i++) {
        if (*p < ' ' || *p > '~')
            return AVERROR_INVALIDDATA;
        n += (*p++ - ' ') * m;
        m *= 95;
    }
    return n;
}
