static int do_iff_chunk_sequence(deark *c, struct de_iffctx *ictx,
	i64 pos1, i64 len, int level)
{
	i64 pos;
	i64 endpos;
	i64 chunk_len;
	struct de_fourcc saved_container_fmt4cc;
	struct de_fourcc saved_container_contentstype4cc;
	int ret;

	if(level >= 16) { // An arbitrary recursion limit.
		return 0;
	}

	endpos = pos1+len;
	saved_container_fmt4cc = ictx->curr_container_fmt4cc;
	saved_container_contentstype4cc = ictx->curr_container_contentstype4cc;

	pos = pos1;
	while(pos < endpos) {
		ictx->curr_container_fmt4cc = saved_container_fmt4cc;
		ictx->curr_container_contentstype4cc = saved_container_contentstype4cc;

		if(ictx->handle_nonchunk_data_fn) {
			i64 skip_len = 0;
			ret = ictx->handle_nonchunk_data_fn(c, ictx, pos, &skip_len);
			if(ret && skip_len>0) {
				pos += de_pad_to_n(skip_len, ictx->alignment);
				continue;
			}
		}

		ret = do_iff_chunk(c, ictx, pos, endpos-pos, level, &chunk_len);
		if(!ret) return 0;
		pos += chunk_len;
	}

	ictx->curr_container_fmt4cc = saved_container_fmt4cc;
	ictx->curr_container_contentstype4cc = saved_container_contentstype4cc;

	return 1;
}