int check_tm_pred4x4_mode(int mode, int mb_x, int mb_y, int vp7)
{
    if (!mb_x) {
        return mb_y ? VERT_VP8_PRED : (vp7 ? DC_128_PRED : DC_129_PRED);
    } else {
        return mb_y ? mode : HOR_VP8_PRED;
    }
}
