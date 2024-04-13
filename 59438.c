lzh_huffman_free(struct huffman *hf)
{
	free(hf->bitlen);
	free(hf->tbl);
	free(hf->tree);
}
