lzh_decode_free(struct lzh_stream *strm)
{

	if (strm->ds == NULL)
		return;
	free(strm->ds->w_buff);
	lzh_huffman_free(&(strm->ds->lt));
	lzh_huffman_free(&(strm->ds->pt));
	free(strm->ds);
	strm->ds = NULL;
}
