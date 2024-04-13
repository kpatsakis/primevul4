void decode_intra4x4_modes(VP8Context *s, VP56RangeCoder *c, VP8Macroblock *mb,
                           int mb_x, int keyframe, int layout)
{
    uint8_t *intra4x4 = mb->intra4x4_pred_mode_mb;

    if (layout) {
        VP8Macroblock *mb_top = mb - s->mb_width - 1;
        memcpy(mb->intra4x4_pred_mode_top, mb_top->intra4x4_pred_mode_top, 4);
    }
    if (keyframe) {
        int x, y;
        uint8_t *top;
        uint8_t *const left = s->intra4x4_pred_mode_left;
        if (layout)
            top = mb->intra4x4_pred_mode_top;
        else
            top = s->intra4x4_pred_mode_top + 4 * mb_x;
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 4; x++) {
                const uint8_t *ctx;
                ctx       = vp8_pred4x4_prob_intra[top[x]][left[y]];
                *intra4x4 = vp8_rac_get_tree(c, vp8_pred4x4_tree, ctx);
                left[y]   = top[x] = *intra4x4;
                intra4x4++;
            }
        }
    } else {
        int i;
        for (i = 0; i < 16; i++)
            intra4x4[i] = vp8_rac_get_tree(c, vp8_pred4x4_tree,
                                           vp8_pred4x4_prob_inter);
    }
}
