int check_intra_pred8x8_mode_emuedge(int mode, int mb_x, int mb_y, int vp7)
{
    switch (mode) {
    case DC_PRED8x8:
        return check_dc_pred8x8_mode(mode, mb_x, mb_y);
    case VERT_PRED8x8:
        return !mb_y ? (vp7 ? DC_128_PRED8x8 : DC_127_PRED8x8) : mode;
    case HOR_PRED8x8:
        return !mb_x ? (vp7 ? DC_128_PRED8x8 : DC_129_PRED8x8) : mode;
    case PLANE_PRED8x8: /* TM */
        return check_tm_pred8x8_mode(mode, mb_x, mb_y, vp7);
    }
    return mode;
}
