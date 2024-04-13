static void inv_predict_6(uint8_t *p, const uint8_t *p_l, const uint8_t *p_tl,
                          const uint8_t *p_t, const uint8_t *p_tr)
{
    p[0] = p_l[0] + p_tl[0] >> 1;
    p[1] = p_l[1] + p_tl[1] >> 1;
    p[2] = p_l[2] + p_tl[2] >> 1;
    p[3] = p_l[3] + p_tl[3] >> 1;
}
