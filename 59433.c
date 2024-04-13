lzh_decode_blocks(struct lzh_stream *strm, int last)
{
	struct lzh_dec *ds = strm->ds;
	struct lzh_br bre = ds->br;
	struct huffman *lt = &(ds->lt);
	struct huffman *pt = &(ds->pt);
	unsigned char *w_buff = ds->w_buff;
	unsigned char *lt_bitlen = lt->bitlen;
	unsigned char *pt_bitlen = pt->bitlen;
	int blocks_avail = ds->blocks_avail, c = 0;
	int copy_len = ds->copy_len, copy_pos = ds->copy_pos;
	int w_pos = ds->w_pos, w_mask = ds->w_mask, w_size = ds->w_size;
	int lt_max_bits = lt->max_bits, pt_max_bits = pt->max_bits;
	int state = ds->state;

	for (;;) {
		switch (state) {
		case ST_GET_LITERAL:
			for (;;) {
				if (blocks_avail == 0) {
					/* We have decoded all blocks.
					 * Let's handle next blocks. */
					ds->state = ST_RD_BLOCK;
					ds->br = bre;
					ds->blocks_avail = 0;
					ds->w_pos = w_pos;
					ds->copy_pos = 0;
					return (100);
				}

				/* lzh_br_read_ahead() always try to fill the
				 * cache buffer up. In specific situation we
				 * are close to the end of the data, the cache
				 * buffer will not be full and thus we have to
				 * determine if the cache buffer has some bits
				 * as much as we need after lzh_br_read_ahead()
				 * failed. */
				if (!lzh_br_read_ahead(strm, &bre,
				    lt_max_bits)) {
					if (!last)
						goto next_data;
					/* Remaining bits are less than
					 * maximum bits(lt.max_bits) but maybe
					 * it still remains as much as we need,
					 * so we should try to use it with
					 * dummy bits. */
					c = lzh_decode_huffman(lt,
					      lzh_br_bits_forced(&bre,
					        lt_max_bits));
					lzh_br_consume(&bre, lt_bitlen[c]);
					if (!lzh_br_has(&bre, 0))
						goto failed;/* Over read. */
				} else {
					c = lzh_decode_huffman(lt,
					      lzh_br_bits(&bre, lt_max_bits));
					lzh_br_consume(&bre, lt_bitlen[c]);
				}
				blocks_avail--;
				if (c > UCHAR_MAX)
					/* Current block is a match data. */
					break;
				/*
				 * 'c' is exactly a literal code.
				 */
				/* Save a decoded code to reference it
				 * afterward. */
				w_buff[w_pos] = c;
				if (++w_pos >= w_size) {
					w_pos = 0;
					lzh_emit_window(strm, w_size);
					goto next_data;
				}
			}
			/* 'c' is the length of a match pattern we have
			 * already extracted, which has be stored in
			 * window(ds->w_buff). */
			copy_len = c - (UCHAR_MAX + 1) + MINMATCH;
			/* FALL THROUGH */
		case ST_GET_POS_1:
			/*
			 * Get a reference position. 
			 */
			if (!lzh_br_read_ahead(strm, &bre, pt_max_bits)) {
				if (!last) {
					state = ST_GET_POS_1;
					ds->copy_len = copy_len;
					goto next_data;
				}
				copy_pos = lzh_decode_huffman(pt,
				    lzh_br_bits_forced(&bre, pt_max_bits));
				lzh_br_consume(&bre, pt_bitlen[copy_pos]);
				if (!lzh_br_has(&bre, 0))
					goto failed;/* Over read. */
			} else {
				copy_pos = lzh_decode_huffman(pt,
				    lzh_br_bits(&bre, pt_max_bits));
				lzh_br_consume(&bre, pt_bitlen[copy_pos]);
			}
			/* FALL THROUGH */
		case ST_GET_POS_2:
			if (copy_pos > 1) {
				/* We need an additional adjustment number to
				 * the position. */
				int p = copy_pos - 1;
				if (!lzh_br_read_ahead(strm, &bre, p)) {
					if (last)
						goto failed;/* Truncated data.*/
					state = ST_GET_POS_2;
					ds->copy_len = copy_len;
					ds->copy_pos = copy_pos;
					goto next_data;
				}
				copy_pos = (1 << p) + lzh_br_bits(&bre, p);
				lzh_br_consume(&bre, p);
			}
			/* The position is actually a distance from the last
			 * code we had extracted and thus we have to convert
			 * it to a position of the window. */
			copy_pos = (w_pos - copy_pos - 1) & w_mask;
			/* FALL THROUGH */
		case ST_COPY_DATA:
			/*
			 * Copy `copy_len' bytes as extracted data from
			 * the window into the output buffer.
			 */
			for (;;) {
				int l;

				l = copy_len;
				if (copy_pos > w_pos) {
					if (l > w_size - copy_pos)
						l = w_size - copy_pos;
				} else {
					if (l > w_size - w_pos)
						l = w_size - w_pos;
				}
				if ((copy_pos + l < w_pos)
				    || (w_pos + l < copy_pos)) {
					/* No overlap. */
					memcpy(w_buff + w_pos,
					    w_buff + copy_pos, l);
				} else {
					const unsigned char *s;
					unsigned char *d;
					int li;

					d = w_buff + w_pos;
					s = w_buff + copy_pos;
					for (li = 0; li < l-1;) {
						d[li] = s[li];li++;
						d[li] = s[li];li++;
					}
					if (li < l)
						d[li] = s[li];
				}
				w_pos += l;
				if (w_pos == w_size) {
					w_pos = 0;
					lzh_emit_window(strm, w_size);
					if (copy_len <= l)
						state = ST_GET_LITERAL;
					else {
						state = ST_COPY_DATA;
						ds->copy_len = copy_len - l;
						ds->copy_pos =
						    (copy_pos + l) & w_mask;
					}
					goto next_data;
				}
				if (copy_len <= l)
					/* A copy of current pattern ended. */
					break;
				copy_len -= l;
				copy_pos = (copy_pos + l) & w_mask;
			}
			state = ST_GET_LITERAL;
			break;
		}
	}
failed:
	return (ds->error = ARCHIVE_FAILED);
next_data:
	ds->br = bre;
	ds->blocks_avail = blocks_avail;
	ds->state = state;
	ds->w_pos = w_pos;
	return (ARCHIVE_OK);
}
