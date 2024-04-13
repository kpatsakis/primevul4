static void inverse_prediction(AVFrame *frame, enum PredictionMode m, int x, int y)
{
    uint8_t *dec, *p_l, *p_tl, *p_t, *p_tr;
    uint8_t p[4];

    dec  = GET_PIXEL(frame, x,     y);
    p_l  = GET_PIXEL(frame, x - 1, y);
    p_tl = GET_PIXEL(frame, x - 1, y - 1);
    p_t  = GET_PIXEL(frame, x,     y - 1);
    if (x == frame->width - 1)
        p_tr = GET_PIXEL(frame, 0, y);
    else
        p_tr = GET_PIXEL(frame, x + 1, y - 1);

    inverse_predict[m](p, p_l, p_tl, p_t, p_tr);

    dec[0] += p[0];
    dec[1] += p[1];
    dec[2] += p[2];
    dec[3] += p[3];
}
