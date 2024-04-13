static void apply_filter(APEContext *ctx, APEFilter *f,
                         int32_t *data0, int32_t *data1,
                         int count, int order, int fracbits)
{
    do_apply_filter(ctx, ctx->fileversion, &f[0], data0, count, order, fracbits);
    if (data1)
        do_apply_filter(ctx, ctx->fileversion, &f[1], data1, count, order, fracbits);
}
