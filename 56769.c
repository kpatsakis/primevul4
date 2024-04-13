static void entropy_decode_stereo_3930(APEContext *ctx, int blockstodecode)
{
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];

    while (blockstodecode--) {
        *decoded0++ = ape_decode_value_3900(ctx, &ctx->riceY);
        *decoded1++ = ape_decode_value_3900(ctx, &ctx->riceX);
    }
}
