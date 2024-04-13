static void ape_apply_filters(APEContext *ctx, int32_t *decoded0,
                              int32_t *decoded1, int count)
{
    int i;

    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[ctx->fset][i])
            break;
        apply_filter(ctx, ctx->filters[i], decoded0, decoded1, count,
                     ape_filter_orders[ctx->fset][i],
                     ape_filter_fracbits[ctx->fset][i]);
    }
}
