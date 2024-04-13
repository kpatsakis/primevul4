static void parse_segment_info(VP8Context *s)
{
    VP56RangeCoder *c = &s->c;
    int i;

    s->segmentation.update_map = vp8_rac_get(c);

    if (vp8_rac_get(c)) { // update segment feature data
        s->segmentation.absolute_vals = vp8_rac_get(c);

        for (i = 0; i < 4; i++)
            s->segmentation.base_quant[i]   = vp8_rac_get_sint(c, 7);

        for (i = 0; i < 4; i++)
            s->segmentation.filter_level[i] = vp8_rac_get_sint(c, 6);
    }
    if (s->segmentation.update_map)
        for (i = 0; i < 3; i++)
            s->prob->segmentid[i] = vp8_rac_get(c) ? vp8_rac_get_uint(c, 8) : 255;
}
