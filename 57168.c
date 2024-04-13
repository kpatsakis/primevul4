static void alpha_inverse_prediction(AVFrame *frame, enum AlphaFilter m)
{
    int x, y, ls;
    uint8_t *dec;

    ls = frame->linesize[3];

    /* filter first row using horizontal filter */
    dec = frame->data[3] + 1;
    for (x = 1; x < frame->width; x++, dec++)
        *dec += *(dec - 1);

    /* filter first column using vertical filter */
    dec = frame->data[3] + ls;
    for (y = 1; y < frame->height; y++, dec += ls)
        *dec += *(dec - ls);

    /* filter the rest using the specified filter */
    switch (m) {
    case ALPHA_FILTER_HORIZONTAL:
        for (y = 1; y < frame->height; y++) {
            dec = frame->data[3] + y * ls + 1;
            for (x = 1; x < frame->width; x++, dec++)
                *dec += *(dec - 1);
        }
        break;
    case ALPHA_FILTER_VERTICAL:
        for (y = 1; y < frame->height; y++) {
            dec = frame->data[3] + y * ls + 1;
            for (x = 1; x < frame->width; x++, dec++)
                *dec += *(dec - ls);
        }
        break;
    case ALPHA_FILTER_GRADIENT:
        for (y = 1; y < frame->height; y++) {
            dec = frame->data[3] + y * ls + 1;
            for (x = 1; x < frame->width; x++, dec++)
                dec[0] += av_clip_uint8(*(dec - 1) + *(dec - ls) - *(dec - ls - 1));
        }
        break;
    }
}
