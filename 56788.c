static inline void range_decode_update(APEContext *ctx, int sy_f, int lt_f)
{
    ctx->rc.low  -= ctx->rc.help * lt_f;
    ctx->rc.range = ctx->rc.help * sy_f;
}
