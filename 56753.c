static inline int ape_decode_value_3860(APEContext *ctx, GetBitContext *gb,
                                        APERice *rice)
{
    unsigned int x, overflow;

    overflow = get_unary(gb, 1, get_bits_left(gb));

    if (ctx->fileversion > 3880) {
        while (overflow >= 16) {
            overflow -= 16;
            rice->k  += 4;
        }
    }

    if (!rice->k)
        x = overflow;
    else if(rice->k <= MIN_CACHE_BITS) {
        x = (overflow << rice->k) + get_bits(gb, rice->k);
    } else {
        av_log(ctx->avctx, AV_LOG_ERROR, "Too many bits: %"PRIu32"\n", rice->k);
        return AVERROR_INVALIDDATA;
    }
    rice->ksum += x - (rice->ksum + 8 >> 4);
    if (rice->ksum < (rice->k ? 1 << (rice->k + 4) : 0))
        rice->k--;
    else if (rice->ksum >= (1 << (rice->k + 5)) && rice->k < 24)
        rice->k++;

    /* Convert to signed */
    return ((x >> 1) ^ ((x & 1) - 1)) + 1;
}
