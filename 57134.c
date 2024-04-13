static void update_refs(VP8Context *s)
{
    VP56RangeCoder *c = &s->c;

    int update_golden = vp8_rac_get(c);
    int update_altref = vp8_rac_get(c);

    s->update_golden = ref_to_update(s, update_golden, VP56_FRAME_GOLDEN);
    s->update_altref = ref_to_update(s, update_altref, VP56_FRAME_GOLDEN2);
}
