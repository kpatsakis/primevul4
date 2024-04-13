static void ape_unpack_stereo(APEContext *ctx, int count)
{
    int32_t left, right;
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];

    if ((ctx->frameflags & APE_FRAMECODE_STEREO_SILENCE) == APE_FRAMECODE_STEREO_SILENCE) {
        /* We are pure silence, so we're done. */
        av_log(ctx->avctx, AV_LOG_DEBUG, "pure silence stereo\n");
        return;
    }

    ctx->entropy_decode_stereo(ctx, count);

    /* Now apply the predictor decoding */
    ctx->predictor_decode_stereo(ctx, count);

    /* Decorrelate and scale to output depth */
    while (count--) {
        left = *decoded1 - (*decoded0 / 2);
        right = left + *decoded0;

        *(decoded0++) = left;
        *(decoded1++) = right;
    }
}
