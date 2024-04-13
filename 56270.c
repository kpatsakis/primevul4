static void bmp_mask_get_shift_and_prec(OPJ_UINT32 mask, OPJ_UINT32* shift,
                                        OPJ_UINT32* prec)
{
    OPJ_UINT32 l_shift, l_prec;

    l_shift = l_prec = 0U;

    if (mask != 0U) {
        while ((mask & 1U) == 0U) {
            mask >>= 1;
            l_shift++;
        }
        while (mask & 1U) {
            mask >>= 1;
            l_prec++;
        }
    }
    *shift = l_shift;
    *prec = l_prec;
}
