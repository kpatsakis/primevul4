static void entropy_decode_stereo_3860(APEContext *ctx, int blockstodecode)
{
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];
    int blocks = blockstodecode;

    while (blockstodecode--)
        *decoded0++ = ape_decode_value_3860(ctx, &ctx->gb, &ctx->riceY);
    while (blocks--)
        *decoded1++ = ape_decode_value_3860(ctx, &ctx->gb, &ctx->riceX);
}
