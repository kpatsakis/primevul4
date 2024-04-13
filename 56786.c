static inline int range_decode_culfreq(APEContext *ctx, int tot_f)
{
    range_dec_normalize(ctx);
    ctx->rc.help = ctx->rc.range / tot_f;
    return ctx->rc.low / ctx->rc.help;
}
