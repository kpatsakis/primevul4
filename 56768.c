static void entropy_decode_stereo_3900(APEContext *ctx, int blockstodecode)
{
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];
    int blocks = blockstodecode;

    while (blockstodecode--)
        *decoded0++ = ape_decode_value_3900(ctx, &ctx->riceY);
    range_dec_normalize(ctx);
    ctx->ptr -= 1;
    range_start_decoding(ctx);
    while (blocks--)
        *decoded1++ = ape_decode_value_3900(ctx, &ctx->riceX);
}
