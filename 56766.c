static void entropy_decode_stereo_0000(APEContext *ctx, int blockstodecode)
{
    decode_array_0000(ctx, &ctx->gb, ctx->decoded[0], &ctx->riceY,
                      blockstodecode);
    decode_array_0000(ctx, &ctx->gb, ctx->decoded[1], &ctx->riceX,
                      blockstodecode);
}
