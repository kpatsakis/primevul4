static inline int range_decode_culshift(APEContext *ctx, int shift)
{
    range_dec_normalize(ctx);
    ctx->rc.help = ctx->rc.range >> shift;
    return ctx->rc.low / ctx->rc.help;
}
