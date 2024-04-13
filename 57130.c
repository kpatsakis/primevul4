static VP56Frame ref_to_update(VP8Context *s, int update, VP56Frame ref)
{
    VP56RangeCoder *c = &s->c;

    if (update)
        return VP56_FRAME_CURRENT;

    switch (vp8_rac_get_uint(c, 2)) {
    case 1:
        return VP56_FRAME_PREVIOUS;
    case 2:
        return (ref == VP56_FRAME_GOLDEN) ? VP56_FRAME_GOLDEN2 : VP56_FRAME_GOLDEN;
    }
    return VP56_FRAME_NONE;
}
