static void do_box_sequence(deark *c, struct de_boxesctx *bctx,
	i64 pos1, i64 len, i64 max_nboxes, int level)
{
	i64 pos;
	i64 box_len;
	i64 endpos;
	int ret;
	i64 box_count = 0;

	if(level >= 32) { // An arbitrary recursion limit.
		return;
	}

	pos = pos1;
	endpos = pos1 + len;

	while(pos < endpos) {
		if(max_nboxes>=0 && box_count>=max_nboxes) break;
		ret = do_box(c, bctx, pos, endpos-pos, level, &box_len);
		if(!ret) break;
		box_count++;
		pos += box_len;
	}
}