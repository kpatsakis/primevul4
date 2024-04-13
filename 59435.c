lzh_decode_huffman(struct huffman *hf, unsigned rbits)
{
	int c;
	/*
	 * At first search an index table for a bit pattern.
	 * If it fails, search a huffman tree for.
	 */
	c = hf->tbl[rbits >> hf->shift_bits];
	if (c < hf->len_avail || hf->len_avail == 0)
		return (c);
	/* This bit pattern needs to be found out at a huffman tree. */
	return (lzh_decode_huffman_tree(hf, rbits, c));
}
