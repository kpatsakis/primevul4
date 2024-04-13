static void init_predictor_decoder(APEContext *ctx)
{
    APEPredictor *p = &ctx->predictor;

    /* Zero the history buffers */
    memset(p->historybuffer, 0, PREDICTOR_SIZE * sizeof(*p->historybuffer));
    p->buf = p->historybuffer;

    /* Initialize and zero the coefficients */
    if (ctx->fileversion < 3930) {
        if (ctx->compression_level == COMPRESSION_LEVEL_FAST) {
            memcpy(p->coeffsA[0], initial_coeffs_fast_3320,
                   sizeof(initial_coeffs_fast_3320));
            memcpy(p->coeffsA[1], initial_coeffs_fast_3320,
                   sizeof(initial_coeffs_fast_3320));
        } else {
            memcpy(p->coeffsA[0], initial_coeffs_a_3800,
                   sizeof(initial_coeffs_a_3800));
            memcpy(p->coeffsA[1], initial_coeffs_a_3800,
                   sizeof(initial_coeffs_a_3800));
        }
    } else {
        memcpy(p->coeffsA[0], initial_coeffs_3930, sizeof(initial_coeffs_3930));
        memcpy(p->coeffsA[1], initial_coeffs_3930, sizeof(initial_coeffs_3930));
    }
    memset(p->coeffsB, 0, sizeof(p->coeffsB));
    if (ctx->fileversion < 3930) {
        memcpy(p->coeffsB[0], initial_coeffs_b_3800,
               sizeof(initial_coeffs_b_3800));
        memcpy(p->coeffsB[1], initial_coeffs_b_3800,
               sizeof(initial_coeffs_b_3800));
    }

    p->filterA[0] = p->filterA[1] = 0;
    p->filterB[0] = p->filterB[1] = 0;
    p->lastA[0]   = p->lastA[1]   = 0;

    p->sample_pos = 0;
}
