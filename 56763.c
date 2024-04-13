static void entropy_decode_mono_3860(APEContext *ctx, int blockstodecode)
{
    int32_t *decoded0 = ctx->decoded[0];

    while (blockstodecode--)
        *decoded0++ = ape_decode_value_3860(ctx, &ctx->gb, &ctx->riceY);
}
