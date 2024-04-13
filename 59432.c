lzh_decode(struct lzh_stream *strm, int last)
{
	struct lzh_dec *ds = strm->ds;
	int avail_in;
	int r;

	if (ds->error)
		return (ds->error);

	avail_in = strm->avail_in;
	do {
		if (ds->state < ST_GET_LITERAL)
			r = lzh_read_blocks(strm, last);
		else
			r = lzh_decode_blocks(strm, last);
	} while (r == 100);
	strm->total_in += avail_in - strm->avail_in;
	return (r);
}
