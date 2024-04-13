int check_dc_pred8x8_mode(int mode, int mb_x, int mb_y)
{
    if (!mb_x)
        return mb_y ? TOP_DC_PRED8x8 : DC_128_PRED8x8;
    else
        return mb_y ? mode : LEFT_DC_PRED8x8;
}
