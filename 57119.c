void filter_mb_simple(VP8Context *s, uint8_t *dst, VP8FilterStrength *f,
                      int mb_x, int mb_y)
{
    int mbedge_lim, bedge_lim;
    int filter_level = f->filter_level;
    int inner_limit  = f->inner_limit;
    int inner_filter = f->inner_filter;
    ptrdiff_t linesize = s->linesize;

    if (!filter_level)
        return;

    bedge_lim  = 2 * filter_level + inner_limit;
    mbedge_lim = bedge_lim + 4;

    if (mb_x)
        s->vp8dsp.vp8_h_loop_filter_simple(dst, linesize, mbedge_lim);
    if (inner_filter) {
        s->vp8dsp.vp8_h_loop_filter_simple(dst +  4, linesize, bedge_lim);
        s->vp8dsp.vp8_h_loop_filter_simple(dst +  8, linesize, bedge_lim);
        s->vp8dsp.vp8_h_loop_filter_simple(dst + 12, linesize, bedge_lim);
    }

    if (mb_y)
        s->vp8dsp.vp8_v_loop_filter_simple(dst, linesize, mbedge_lim);
    if (inner_filter) {
        s->vp8dsp.vp8_v_loop_filter_simple(dst +  4 * linesize, linesize, bedge_lim);
        s->vp8dsp.vp8_v_loop_filter_simple(dst +  8 * linesize, linesize, bedge_lim);
        s->vp8dsp.vp8_v_loop_filter_simple(dst + 12 * linesize, linesize, bedge_lim);
    }
}
