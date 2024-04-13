static void inv_predict_13(uint8_t *p, const uint8_t *p_l, const uint8_t *p_tl,
                           const uint8_t *p_t, const uint8_t *p_tr)
{
    p[0] = clamp_add_subtract_half(p_l[0], p_t[0], p_tl[0]);
    p[1] = clamp_add_subtract_half(p_l[1], p_t[1], p_tl[1]);
    p[2] = clamp_add_subtract_half(p_l[2], p_t[2], p_tl[2]);
    p[3] = clamp_add_subtract_half(p_l[3], p_t[3], p_tl[3]);
}
