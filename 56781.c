static void predictor_decode_stereo_3930(APEContext *ctx, int count)
{
    APEPredictor *p = &ctx->predictor;
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];

    ape_apply_filters(ctx, ctx->decoded[0], ctx->decoded[1], count);

    while (count--) {
        /* Predictor Y */
        int Y = *decoded1, X = *decoded0;
        *decoded0 = predictor_update_3930(p, Y, 0, YDELAYA);
        decoded0++;
        *decoded1 = predictor_update_3930(p, X, 1, XDELAYA);
        decoded1++;

        /* Combined */
        p->buf++;

        /* Have we filled the history buffer? */
        if (p->buf == p->historybuffer + HISTORY_SIZE) {
            memmove(p->historybuffer, p->buf,
                    PREDICTOR_SIZE * sizeof(*p->historybuffer));
            p->buf = p->historybuffer;
        }
    }
}
