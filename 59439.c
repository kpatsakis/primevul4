lzh_huffman_init(struct huffman *hf, size_t len_size, int tbl_bits)
{
	int bits;

	if (hf->bitlen == NULL) {
		hf->bitlen = malloc(len_size * sizeof(hf->bitlen[0]));
		if (hf->bitlen == NULL)
			return (ARCHIVE_FATAL);
	}
	if (hf->tbl == NULL) {
		if (tbl_bits < HTBL_BITS)
			bits = tbl_bits;
		else
			bits = HTBL_BITS;
		hf->tbl = malloc(((size_t)1 << bits) * sizeof(hf->tbl[0]));
		if (hf->tbl == NULL)
			return (ARCHIVE_FATAL);
	}
	if (hf->tree == NULL && tbl_bits > HTBL_BITS) {
		hf->tree_avail = 1 << (tbl_bits - HTBL_BITS + 4);
		hf->tree = malloc(hf->tree_avail * sizeof(hf->tree[0]));
		if (hf->tree == NULL)
			return (ARCHIVE_FATAL);
	}
	hf->len_size = (int)len_size;
	hf->tbl_bits = tbl_bits;
	return (ARCHIVE_OK);
}
