int ff_h264_check_intra_pred_mode(H264Context *h, int mode, int is_chroma)
{
    static const int8_t top[4]  = { LEFT_DC_PRED8x8, 1, -1, -1 };
    static const int8_t left[5] = { TOP_DC_PRED8x8, -1,  2, -1, DC_128_PRED8x8 };

    if (mode > 3U) {
        av_log(h->avctx, AV_LOG_ERROR,
               "out of range intra chroma pred mode at %d %d\n",
               h->mb_x, h->mb_y);
        return AVERROR_INVALIDDATA;
    }

    if (!(h->top_samples_available & 0x8000)) {
        mode = top[mode];
        if (mode < 0) {
            av_log(h->avctx, AV_LOG_ERROR,
                   "top block unavailable for requested intra mode at %d %d\n",
                   h->mb_x, h->mb_y);
            return AVERROR_INVALIDDATA;
        }
    }

    if ((h->left_samples_available & 0x8080) != 0x8080) {
        mode = left[mode];
        if (mode < 0) {
            av_log(h->avctx, AV_LOG_ERROR,
                   "left block unavailable for requested intra mode at %d %d\n",
                   h->mb_x, h->mb_y);
            return AVERROR_INVALIDDATA;
        }
        if (is_chroma && (h->left_samples_available & 0x8080)) {
            mode = ALZHEIMER_DC_L0T_PRED8x8 +
                   (!(h->left_samples_available & 0x8000)) +
                   2 * (mode == DC_128_PRED8x8);
        }
    }

    return mode;
}
