static void predictor_decode_mono_3930(APEContext *ctx, int count)
{
    APEPredictor *p = &ctx->predictor;
    int32_t *decoded0 = ctx->decoded[0];

    ape_apply_filters(ctx, ctx->decoded[0], NULL, count);

    while (count--) {
        *decoded0 = predictor_update_3930(p, *decoded0, 0, YDELAYA);
        decoded0++;

        p->buf++;

        /* Have we filled the history buffer? */
        if (p->buf == p->historybuffer + HISTORY_SIZE) {
            memmove(p->historybuffer, p->buf,
                    PREDICTOR_SIZE * sizeof(*p->historybuffer));
            p->buf = p->historybuffer;
        }
    }
}
