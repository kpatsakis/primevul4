static inline int get_rice_ook(GetBitContext *gb, int k)
{
    unsigned int x;

    x = get_unary(gb, 1, get_bits_left(gb));

    if (k)
        x = (x << k) | get_bits(gb, k);

    return x;
}
