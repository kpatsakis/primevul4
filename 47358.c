int ff_h264_alloc_tables(H264Context *h)
{
    const int big_mb_num = h->mb_stride * (h->mb_height + 1);
    const int row_mb_num = 2*h->mb_stride*FFMAX(h->avctx->thread_count, 1);
    int x, y, i;

    FF_ALLOCZ_ARRAY_OR_GOTO(h->avctx, h->intra4x4_pred_mode,
                      row_mb_num, 8 * sizeof(uint8_t), fail)
    FF_ALLOCZ_OR_GOTO(h->avctx, h->non_zero_count,
                      big_mb_num * 48 * sizeof(uint8_t), fail)
    FF_ALLOCZ_OR_GOTO(h->avctx, h->slice_table_base,
                      (big_mb_num + h->mb_stride) * sizeof(*h->slice_table_base), fail)
    FF_ALLOCZ_OR_GOTO(h->avctx, h->cbp_table,
                      big_mb_num * sizeof(uint16_t), fail)
    FF_ALLOCZ_OR_GOTO(h->avctx, h->chroma_pred_mode_table,
                      big_mb_num * sizeof(uint8_t), fail)
    FF_ALLOCZ_ARRAY_OR_GOTO(h->avctx, h->mvd_table[0],
                      row_mb_num, 16 * sizeof(uint8_t), fail);
    FF_ALLOCZ_ARRAY_OR_GOTO(h->avctx, h->mvd_table[1],
                      row_mb_num, 16 * sizeof(uint8_t), fail);
    FF_ALLOCZ_OR_GOTO(h->avctx, h->direct_table,
                      4 * big_mb_num * sizeof(uint8_t), fail);
    FF_ALLOCZ_OR_GOTO(h->avctx, h->list_counts,
                      big_mb_num * sizeof(uint8_t), fail)

    memset(h->slice_table_base, -1,
           (big_mb_num + h->mb_stride) * sizeof(*h->slice_table_base));
    h->slice_table = h->slice_table_base + h->mb_stride * 2 + 1;

    FF_ALLOCZ_OR_GOTO(h->avctx, h->mb2b_xy,
                      big_mb_num * sizeof(uint32_t), fail);
    FF_ALLOCZ_OR_GOTO(h->avctx, h->mb2br_xy,
                      big_mb_num * sizeof(uint32_t), fail);
    for (y = 0; y < h->mb_height; y++)
        for (x = 0; x < h->mb_width; x++) {
            const int mb_xy = x + y * h->mb_stride;
            const int b_xy  = 4 * x + 4 * y * h->b_stride;

            h->mb2b_xy[mb_xy]  = b_xy;
            h->mb2br_xy[mb_xy] = 8 * (FMO ? mb_xy : (mb_xy % (2 * h->mb_stride)));
        }

    if (!h->dequant4_coeff[0])
        h264_init_dequant_tables(h);

    if (!h->DPB) {
        h->DPB = av_mallocz_array(H264_MAX_PICTURE_COUNT, sizeof(*h->DPB));
        if (!h->DPB)
            goto fail;
        for (i = 0; i < H264_MAX_PICTURE_COUNT; i++)
            av_frame_unref(&h->DPB[i].f);
        av_frame_unref(&h->cur_pic.f);
    }

    return 0;

fail:
    ff_h264_free_tables(h, 1);
    return AVERROR(ENOMEM);
}
