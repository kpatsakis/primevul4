int check_intra_pred4x4_mode_emuedge(int mode, int mb_x, int mb_y,
                                     int *copy_buf, int vp7)
{
    switch (mode) {
    case VERT_PRED:
        if (!mb_x && mb_y) {
            *copy_buf = 1;
            return mode;
        }
        /* fall-through */
    case DIAG_DOWN_LEFT_PRED:
    case VERT_LEFT_PRED:
        return !mb_y ? (vp7 ? DC_128_PRED : DC_127_PRED) : mode;
    case HOR_PRED:
        if (!mb_y) {
            *copy_buf = 1;
            return mode;
        }
        /* fall-through */
    case HOR_UP_PRED:
        return !mb_x ? (vp7 ? DC_128_PRED : DC_129_PRED) : mode;
    case TM_VP8_PRED:
        return check_tm_pred4x4_mode(mode, mb_x, mb_y, vp7);
    case DC_PRED: /* 4x4 DC doesn't use the same "H.264-style" exceptions
                   * as 16x16/8x8 DC */
    case DIAG_DOWN_RIGHT_PRED:
    case VERT_RIGHT_PRED:
    case HOR_DOWN_PRED:
        if (!mb_y || !mb_x)
            *copy_buf = 1;
        return mode;
    }
    return mode;
}
