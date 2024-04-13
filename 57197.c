static void read_huffman_code_simple(WebPContext *s, HuffReader *hc)
{
    hc->nb_symbols = get_bits1(&s->gb) + 1;

    if (get_bits1(&s->gb))
        hc->simple_symbols[0] = get_bits(&s->gb, 8);
    else
        hc->simple_symbols[0] = get_bits1(&s->gb);

    if (hc->nb_symbols == 2)
        hc->simple_symbols[1] = get_bits(&s->gb, 8);

    hc->simple = 1;
}
