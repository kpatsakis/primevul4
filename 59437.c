lzh_decode_init(struct lzh_stream *strm, const char *method)
{
	struct lzh_dec *ds;
	int w_bits, w_size;

	if (strm->ds == NULL) {
		strm->ds = calloc(1, sizeof(*strm->ds));
		if (strm->ds == NULL)
			return (ARCHIVE_FATAL);
	}
	ds = strm->ds;
	ds->error = ARCHIVE_FAILED;
	if (method == NULL || method[0] != 'l' || method[1] != 'h')
		return (ARCHIVE_FAILED);
	switch (method[2]) {
	case '5':
		w_bits = 13;/* 8KiB for window */
		break;
	case '6':
		w_bits = 15;/* 32KiB for window */
		break;
	case '7':
		w_bits = 16;/* 64KiB for window */
		break;
	default:
		return (ARCHIVE_FAILED);/* Not supported. */
	}
	ds->error = ARCHIVE_FATAL;
	/* Expand a window size up to 128 KiB for decompressing process
	 * performance whatever its original window size is. */
	ds->w_size = 1U << 17;
	ds->w_mask = ds->w_size -1;
	if (ds->w_buff == NULL) {
		ds->w_buff = malloc(ds->w_size);
		if (ds->w_buff == NULL)
			return (ARCHIVE_FATAL);
	}
	w_size = 1U << w_bits;
	memset(ds->w_buff + ds->w_size - w_size, 0x20, w_size);
	ds->w_pos = 0;
	ds->state = 0;
	ds->pos_pt_len_size = w_bits + 1;
	ds->pos_pt_len_bits = (w_bits == 15 || w_bits == 16)? 5: 4;
	ds->literal_pt_len_size = PT_BITLEN_SIZE;
	ds->literal_pt_len_bits = 5;
	ds->br.cache_buffer = 0;
	ds->br.cache_avail = 0;

	if (lzh_huffman_init(&(ds->lt), LT_BITLEN_SIZE, 16)
	    != ARCHIVE_OK)
		return (ARCHIVE_FATAL);
	ds->lt.len_bits = 9;
	if (lzh_huffman_init(&(ds->pt), PT_BITLEN_SIZE, 16)
	    != ARCHIVE_OK)
		return (ARCHIVE_FATAL);
	ds->error = 0;

	return (ARCHIVE_OK);
}
