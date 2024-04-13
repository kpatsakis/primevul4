static void vp8_get_quants(VP8Context *s)
{
    VP56RangeCoder *c = &s->c;
    int i, base_qi;

    int yac_qi     = vp8_rac_get_uint(c, 7);
    int ydc_delta  = vp8_rac_get_sint(c, 4);
    int y2dc_delta = vp8_rac_get_sint(c, 4);
    int y2ac_delta = vp8_rac_get_sint(c, 4);
    int uvdc_delta = vp8_rac_get_sint(c, 4);
    int uvac_delta = vp8_rac_get_sint(c, 4);

    for (i = 0; i < 4; i++) {
        if (s->segmentation.enabled) {
            base_qi = s->segmentation.base_quant[i];
            if (!s->segmentation.absolute_vals)
                base_qi += yac_qi;
        } else
            base_qi = yac_qi;

        s->qmat[i].luma_qmul[0]    = vp8_dc_qlookup[av_clip_uintp2(base_qi + ydc_delta,  7)];
        s->qmat[i].luma_qmul[1]    = vp8_ac_qlookup[av_clip_uintp2(base_qi,              7)];
        s->qmat[i].luma_dc_qmul[0] = vp8_dc_qlookup[av_clip_uintp2(base_qi + y2dc_delta, 7)] * 2;
        /* 101581>>16 is equivalent to 155/100 */
        s->qmat[i].luma_dc_qmul[1] = vp8_ac_qlookup[av_clip_uintp2(base_qi + y2ac_delta, 7)] * 101581 >> 16;
        s->qmat[i].chroma_qmul[0]  = vp8_dc_qlookup[av_clip_uintp2(base_qi + uvdc_delta, 7)];
        s->qmat[i].chroma_qmul[1]  = vp8_ac_qlookup[av_clip_uintp2(base_qi + uvac_delta, 7)];

        s->qmat[i].luma_dc_qmul[1] = FFMAX(s->qmat[i].luma_dc_qmul[1], 8);
        s->qmat[i].chroma_qmul[0]  = FFMIN(s->qmat[i].chroma_qmul[0], 132);
    }
}
