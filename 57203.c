static av_always_inline int webp_get_vlc(GetBitContext *gb, VLC_TYPE (*table)[2])
{
    int n, nb_bits;
    unsigned int index;
    int code;

    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);

    index = SHOW_UBITS(re, gb, 8);
    index = ff_reverse[index];
    code  = table[index][0];
    n     = table[index][1];

    if (n < 0) {
        LAST_SKIP_BITS(re, gb, 8);
        UPDATE_CACHE(re, gb);

        nb_bits = -n;

        index = SHOW_UBITS(re, gb, nb_bits);
        index = (ff_reverse[index] >> (8 - nb_bits)) + code;
        code  = table[index][0];
        n     = table[index][1];
    }
    SKIP_BITS(re, gb, n);

    CLOSE_READER(re, gb);

    return code;
}
