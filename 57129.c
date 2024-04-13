static av_always_inline int read_mv_component(VP56RangeCoder *c, const uint8_t *p, int vp7)
{
    int bit, x = 0;

    if (vp56_rac_get_prob_branchy(c, p[0])) {
        int i;

        for (i = 0; i < 3; i++)
            x += vp56_rac_get_prob(c, p[9 + i]) << i;
        for (i = (vp7 ? 7 : 9); i > 3; i--)
            x += vp56_rac_get_prob(c, p[9 + i]) << i;
        if (!(x & (vp7 ? 0xF0 : 0xFFF0)) || vp56_rac_get_prob(c, p[12]))
            x += 8;
    } else {
        const uint8_t *ps = p + 2;
        bit = vp56_rac_get_prob(c, *ps);
        ps += 1 + 3 * bit;
        x  += 4 * bit;
        bit = vp56_rac_get_prob(c, *ps);
        ps += 1 + bit;
        x  += 2 * bit;
        x  += vp56_rac_get_prob(c, *ps);
    }

    return (x && vp56_rac_get_prob(c, p[1])) ? -x : x;
}
