void fmtutil_read_iff_format(deark *c, struct de_iffctx *ictx,
	i64 pos, i64 len)
{
	if(!ictx->f || !ictx->handle_chunk_fn) return; // Internal error

	ictx->level = 0;
	fourcc_clear(&ictx->main_fmt4cc);
	fourcc_clear(&ictx->main_contentstype4cc);
	fourcc_clear(&ictx->curr_container_fmt4cc);
	fourcc_clear(&ictx->curr_container_contentstype4cc);
	if(ictx->alignment==0) {
		ictx->alignment = 2;
	}
	if(ictx->sizeof_len==0) {
		ictx->sizeof_len = 4;
	}

	if(ictx->input_encoding==DE_ENCODING_UNKNOWN) {
		ictx->input_encoding = DE_ENCODING_ASCII;
	}

	do_iff_chunk_sequence(c, ictx, pos, len, 0);
}