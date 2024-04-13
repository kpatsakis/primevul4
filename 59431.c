lzh_br_fillup(struct lzh_stream *strm, struct lzh_br *br)
{
	int n = CACHE_BITS - br->cache_avail;

	for (;;) {
		const int x = n >> 3;
		if (strm->avail_in >= x) {
			switch (x) {
			case 8:
				br->cache_buffer =
				    ((uint64_t)strm->next_in[0]) << 56 |
				    ((uint64_t)strm->next_in[1]) << 48 |
				    ((uint64_t)strm->next_in[2]) << 40 |
				    ((uint64_t)strm->next_in[3]) << 32 |
				    ((uint32_t)strm->next_in[4]) << 24 |
				    ((uint32_t)strm->next_in[5]) << 16 |
				    ((uint32_t)strm->next_in[6]) << 8 |
				     (uint32_t)strm->next_in[7];
				strm->next_in += 8;
				strm->avail_in -= 8;
				br->cache_avail += 8 * 8;
				return (1);
			case 7:
				br->cache_buffer =
		 		   (br->cache_buffer << 56) |
				    ((uint64_t)strm->next_in[0]) << 48 |
				    ((uint64_t)strm->next_in[1]) << 40 |
				    ((uint64_t)strm->next_in[2]) << 32 |
				    ((uint32_t)strm->next_in[3]) << 24 |
				    ((uint32_t)strm->next_in[4]) << 16 |
				    ((uint32_t)strm->next_in[5]) << 8 |
				     (uint32_t)strm->next_in[6];
				strm->next_in += 7;
				strm->avail_in -= 7;
				br->cache_avail += 7 * 8;
				return (1);
			case 6:
				br->cache_buffer =
		 		   (br->cache_buffer << 48) |
				    ((uint64_t)strm->next_in[0]) << 40 |
				    ((uint64_t)strm->next_in[1]) << 32 |
				    ((uint32_t)strm->next_in[2]) << 24 |
				    ((uint32_t)strm->next_in[3]) << 16 |
				    ((uint32_t)strm->next_in[4]) << 8 |
				     (uint32_t)strm->next_in[5];
				strm->next_in += 6;
				strm->avail_in -= 6;
				br->cache_avail += 6 * 8;
				return (1);
			case 0:
				/* We have enough compressed data in
				 * the cache buffer.*/
				return (1);
			default:
				break;
			}
		}
		if (strm->avail_in == 0) {
			/* There is not enough compressed data to fill up the
			 * cache buffer. */
			return (0);
		}
		br->cache_buffer =
		   (br->cache_buffer << 8) | *strm->next_in++;
		strm->avail_in--;
		br->cache_avail += 8;
		n -= 8;
	}
}
