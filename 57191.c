static void inv_predict_9(uint8_t *p, const uint8_t *p_l, const uint8_t *p_tl,
                          const uint8_t *p_t, const uint8_t *p_tr)
{
    p[0] = p_t[0] + p_tr[0] >> 1;
    p[1] = p_t[1] + p_tr[1] >> 1;
    p[2] = p_t[2] + p_tr[2] >> 1;
    p[3] = p_t[3] + p_tr[3] >> 1;
}
