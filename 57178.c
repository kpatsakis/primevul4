static int huff_reader_get_symbol(HuffReader *r, GetBitContext *gb)
{
    if (r->simple) {
        if (r->nb_symbols == 1)
            return r->simple_symbols[0];
        else
            return r->simple_symbols[get_bits1(gb)];
    } else
        return webp_get_vlc(gb, r->vlc.table);
}
