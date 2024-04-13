lzh_decode_huffman_tree(struct huffman *hf, unsigned rbits, int c)
{
	struct htree_t *ht;
	int extlen;

	ht = hf->tree;
	extlen = hf->shift_bits;
	while (c >= hf->len_avail) {
		c -= hf->len_avail;
		if (extlen-- <= 0 || c >= hf->tree_used)
			return (0);
		if (rbits & (1U << extlen))
			c = ht[c].left;
		else
			c = ht[c].right;
	}
	return (c);
}
