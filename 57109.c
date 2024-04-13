void decode_mb_coeffs(VP8Context *s, VP8ThreadData *td, VP56RangeCoder *c,
                      VP8Macroblock *mb, uint8_t t_nnz[9], uint8_t l_nnz[9],
                      int is_vp7)
{
    int i, x, y, luma_start = 0, luma_ctx = 3;
    int nnz_pred, nnz, nnz_total = 0;
    int segment = mb->segment;
    int block_dc = 0;

    if (mb->mode != MODE_I4x4 && (is_vp7 || mb->mode != VP8_MVMODE_SPLIT)) {
        nnz_pred = t_nnz[8] + l_nnz[8];

        nnz = decode_block_coeffs(c, td->block_dc, s->prob->token[1], 0,
                                  nnz_pred, s->qmat[segment].luma_dc_qmul,
                                  ff_zigzag_scan, is_vp7);
        l_nnz[8] = t_nnz[8] = !!nnz;

        if (is_vp7 && mb->mode > MODE_I4x4) {
            nnz |=  inter_predict_dc(td->block_dc,
                                     s->inter_dc_pred[mb->ref_frame - 1]);
        }

        if (nnz) {
            nnz_total += nnz;
            block_dc   = 1;
            if (nnz == 1)
                s->vp8dsp.vp8_luma_dc_wht_dc(td->block, td->block_dc);
            else
                s->vp8dsp.vp8_luma_dc_wht(td->block, td->block_dc);
        }
        luma_start = 1;
        luma_ctx   = 0;
    }

    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++) {
            nnz_pred = l_nnz[y] + t_nnz[x];
            nnz = decode_block_coeffs(c, td->block[y][x],
                                      s->prob->token[luma_ctx],
                                      luma_start, nnz_pred,
                                      s->qmat[segment].luma_qmul,
                                      s->prob[0].scan, is_vp7);
            /* nnz+block_dc may be one more than the actual last index,
             * but we don't care */
            td->non_zero_count_cache[y][x] = nnz + block_dc;
            t_nnz[x] = l_nnz[y] = !!nnz;
            nnz_total += nnz;
        }

    for (i = 4; i < 6; i++)
        for (y = 0; y < 2; y++)
            for (x = 0; x < 2; x++) {
                nnz_pred = l_nnz[i + 2 * y] + t_nnz[i + 2 * x];
                nnz = decode_block_coeffs(c, td->block[i][(y << 1) + x],
                                          s->prob->token[2], 0, nnz_pred,
                                          s->qmat[segment].chroma_qmul,
                                          s->prob[0].scan, is_vp7);
                td->non_zero_count_cache[i][(y << 1) + x] = nnz;
                t_nnz[i + 2 * x] = l_nnz[i + 2 * y] = !!nnz;
                nnz_total += nnz;
            }

    if (!nnz_total)
        mb->skip = 1;
}
