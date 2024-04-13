void intra_predict(VP8Context *s, VP8ThreadData *td, uint8_t *dst[3],
                   VP8Macroblock *mb, int mb_x, int mb_y, int is_vp7)
{
    int x, y, mode, nnz;
    uint32_t tr;

    /* for the first row, we need to run xchg_mb_border to init the top edge
     * to 127 otherwise, skip it if we aren't going to deblock */
    if (mb_y && (s->deblock_filter || !mb_y) && td->thread_nr == 0)
        xchg_mb_border(s->top_border[mb_x + 1], dst[0], dst[1], dst[2],
                       s->linesize, s->uvlinesize, mb_x, mb_y, s->mb_width,
                       s->filter.simple, 1);

    if (mb->mode < MODE_I4x4) {
        mode = check_intra_pred8x8_mode_emuedge(mb->mode, mb_x, mb_y, is_vp7);
        s->hpc.pred16x16[mode](dst[0], s->linesize);
    } else {
        uint8_t *ptr = dst[0];
        uint8_t *intra4x4 = mb->intra4x4_pred_mode_mb;
        const uint8_t lo = is_vp7 ? 128 : 127;
        const uint8_t hi = is_vp7 ? 128 : 129;
        uint8_t tr_top[4] = { lo, lo, lo, lo };

        uint8_t *tr_right = ptr - s->linesize + 16;

        if (mb_y && mb_x == s->mb_width - 1) {
            tr       = tr_right[-1] * 0x01010101u;
            tr_right = (uint8_t *) &tr;
        }

        if (mb->skip)
            AV_ZERO128(td->non_zero_count_cache);

        for (y = 0; y < 4; y++) {
            uint8_t *topright = ptr + 4 - s->linesize;
            for (x = 0; x < 4; x++) {
                int copy = 0;
                ptrdiff_t linesize = s->linesize;
                uint8_t *dst = ptr + 4 * x;
                LOCAL_ALIGNED(4, uint8_t, copy_dst, [5 * 8]);

                if ((y == 0 || x == 3) && mb_y == 0) {
                    topright = tr_top;
                } else if (x == 3)
                    topright = tr_right;

                mode = check_intra_pred4x4_mode_emuedge(intra4x4[x], mb_x + x,
                                                        mb_y + y, &copy, is_vp7);
                if (copy) {
                    dst      = copy_dst + 12;
                    linesize = 8;
                    if (!(mb_y + y)) {
                        copy_dst[3] = lo;
                        AV_WN32A(copy_dst + 4, lo * 0x01010101U);
                    } else {
                        AV_COPY32(copy_dst + 4, ptr + 4 * x - s->linesize);
                        if (!(mb_x + x)) {
                            copy_dst[3] = hi;
                        } else {
                            copy_dst[3] = ptr[4 * x - s->linesize - 1];
                        }
                    }
                    if (!(mb_x + x)) {
                        copy_dst[11] =
                        copy_dst[19] =
                        copy_dst[27] =
                        copy_dst[35] = hi;
                    } else {
                        copy_dst[11] = ptr[4 * x                   - 1];
                        copy_dst[19] = ptr[4 * x + s->linesize     - 1];
                        copy_dst[27] = ptr[4 * x + s->linesize * 2 - 1];
                        copy_dst[35] = ptr[4 * x + s->linesize * 3 - 1];
                    }
                }
                s->hpc.pred4x4[mode](dst, topright, linesize);
                if (copy) {
                    AV_COPY32(ptr + 4 * x,                   copy_dst + 12);
                    AV_COPY32(ptr + 4 * x + s->linesize,     copy_dst + 20);
                    AV_COPY32(ptr + 4 * x + s->linesize * 2, copy_dst + 28);
                    AV_COPY32(ptr + 4 * x + s->linesize * 3, copy_dst + 36);
                }

                nnz = td->non_zero_count_cache[y][x];
                if (nnz) {
                    if (nnz == 1)
                        s->vp8dsp.vp8_idct_dc_add(ptr + 4 * x,
                                                  td->block[y][x], s->linesize);
                    else
                        s->vp8dsp.vp8_idct_add(ptr + 4 * x,
                                               td->block[y][x], s->linesize);
                }
                topright += 4;
            }

            ptr      += 4 * s->linesize;
            intra4x4 += 4;
        }
    }

    mode = check_intra_pred8x8_mode_emuedge(mb->chroma_pred_mode,
                                            mb_x, mb_y, is_vp7);
    s->hpc.pred8x8[mode](dst[1], s->uvlinesize);
    s->hpc.pred8x8[mode](dst[2], s->uvlinesize);

    if (mb_y && (s->deblock_filter || !mb_y) && td->thread_nr == 0)
        xchg_mb_border(s->top_border[mb_x + 1], dst[0], dst[1], dst[2],
                       s->linesize, s->uvlinesize, mb_x, mb_y, s->mb_width,
                       s->filter.simple, 0);
}
