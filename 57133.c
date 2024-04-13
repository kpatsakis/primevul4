static void update_lf_deltas(VP8Context *s)
{
    VP56RangeCoder *c = &s->c;
    int i;

    for (i = 0; i < 4; i++) {
        if (vp8_rac_get(c)) {
            s->lf_delta.ref[i] = vp8_rac_get_uint(c, 6);

            if (vp8_rac_get(c))
                s->lf_delta.ref[i] = -s->lf_delta.ref[i];
        }
    }

    for (i = MODE_I4x4; i <= VP8_MVMODE_SPLIT; i++) {
        if (vp8_rac_get(c)) {
            s->lf_delta.mode[i] = vp8_rac_get_uint(c, 6);

            if (vp8_rac_get(c))
                s->lf_delta.mode[i] = -s->lf_delta.mode[i];
        }
    }
}
