lzh_make_fake_table(struct huffman *hf, uint16_t c)
{
	if (c >= hf->len_size)
		return (0);
	hf->tbl[0] = c;
	hf->max_bits = 0;
	hf->shift_bits = 0;
	hf->bitlen[hf->tbl[0]] = 0;
	return (1);
}
