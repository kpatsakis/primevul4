static inline int range_decode_bits(APEContext *ctx, int n)
{
    int sym = range_decode_culshift(ctx, n);
    range_decode_update(ctx, 1, sym);
    return sym;
}
