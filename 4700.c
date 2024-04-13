static int do_iff_chunk(deark *c, struct de_iffctx *ictx, i64 pos, i64 bytes_avail,
	int level, i64 *pbytes_consumed)
{
	int ret;
	i64 chunk_dlen_raw;
	i64 chunk_dlen_padded;
	i64 data_bytes_avail;
	i64 hdrsize;
	struct de_iffchunkctx chunkctx;
	int saved_indent_level;
	int retval = 0;
	char name_str[80];

	de_zeromem(&chunkctx, sizeof(struct de_iffchunkctx));

	de_dbg_indent_save(c, &saved_indent_level);

	hdrsize = 4+ictx->sizeof_len;
	if(bytes_avail<hdrsize) {
		de_warn(c, "Ignoring %"I64_FMT" bytes at %"I64_FMT"; too small "
			"to be a chunk", bytes_avail, pos);
		goto done;
	}
	data_bytes_avail = bytes_avail-hdrsize;

	dbuf_read_fourcc(ictx->f, pos, &chunkctx.chunk4cc, 4,
		ictx->reversed_4cc ? DE_4CCFLAG_REVERSED : 0x0);
	if(chunkctx.chunk4cc.id==0 && level==0) {
		de_warn(c, "Chunk ID not found at %"I64_FMT"; assuming the data ends "
			"here", pos);
		goto done;
	}

	if(ictx->sizeof_len==2) {
		chunk_dlen_raw = dbuf_getu16x(ictx->f, pos+4, ictx->is_le);
	}
	else {
		chunk_dlen_raw = dbuf_getu32x(ictx->f, pos+4, ictx->is_le);
	}
	chunkctx.dlen = chunk_dlen_raw;
	chunkctx.dpos = pos+hdrsize;

	// TODO: Setting these fields (prior to the identify function) is enough
	// for now, but we should also set the other fields here if we can.
	ictx->level = level;
	ictx->chunkctx = &chunkctx;

	if(ictx->preprocess_chunk_fn) {
		ictx->preprocess_chunk_fn(c, ictx);
	}

	if(chunkctx.chunk_name) {
		de_snprintf(name_str, sizeof(name_str), " (%s)", chunkctx.chunk_name);
	}
	else {
		name_str[0] = '\0';
	}

	de_dbg(c, "chunk '%s'%s at %"I64_FMT", dpos=%"I64_FMT", dlen=%"I64_FMT,
		chunkctx.chunk4cc.id_dbgstr, name_str, pos,
		chunkctx.dpos, chunkctx.dlen);
	de_dbg_indent(c, 1);

	if(chunkctx.dlen > data_bytes_avail) {
		int should_warn = 1;

		if(chunkctx.chunk4cc.id==CODE_RIFF && pos==0 && bytes_avail==ictx->f->len) {
			// Hack:
			// This apparent error, in which the RIFF chunk's length field gives the
			// length of the entire file, is too common (particularly in .ani files)
			// to warn about.
			should_warn = 0;
		}

		if(should_warn) {
			de_warn(c, "Invalid oversized chunk, or unexpected end of file "
				"(chunk at %d ends at %" I64_FMT ", "
				"parent ends at %" I64_FMT ")",
				(int)pos, chunkctx.dlen+chunkctx.dpos, pos+bytes_avail);
		}

		chunkctx.dlen = data_bytes_avail; // Try to continue
		de_dbg(c, "adjusting chunk data len to %"I64_FMT, chunkctx.dlen);
	}

	chunk_dlen_padded = de_pad_to_n(chunkctx.dlen, ictx->alignment);
	*pbytes_consumed = hdrsize + chunk_dlen_padded;

	// We've set *pbytes_consumed, so we can return "success"
	retval = 1;

	// Set ictx fields, prior to calling the handler
	chunkctx.pos = pos;
	chunkctx.len = bytes_avail;
	ictx->handled = 0;
	ictx->is_std_container = 0;
	ictx->is_raw_container = 0;

	ret = ictx->handle_chunk_fn(c, ictx);
	if(!ret) {
		retval = 0;
		goto done;
	}

	if(ictx->is_std_container || ictx->is_raw_container) {
		i64 contents_dpos, contents_dlen;

		ictx->chunkctx = NULL;
		ictx->curr_container_fmt4cc = chunkctx.chunk4cc;
		fourcc_clear(&ictx->curr_container_contentstype4cc);

		if(ictx->is_std_container) {
			contents_dpos = chunkctx.dpos+4;
			contents_dlen = chunkctx.dlen-4;

			// First 4 bytes of payload are the "contents type" or "FORM type"
			dbuf_read_fourcc(ictx->f, chunkctx.dpos, &ictx->curr_container_contentstype4cc, 4,
				ictx->reversed_4cc ? DE_4CCFLAG_REVERSED : 0);

			if(level==0) {
				ictx->main_fmt4cc = ictx->curr_container_fmt4cc;
				ictx->main_contentstype4cc = ictx->curr_container_contentstype4cc; // struct copy
			}
			de_dbg(c, "contents type: '%s'", ictx->curr_container_contentstype4cc.id_dbgstr);

			if(ictx->on_std_container_start_fn) {
				// Call only for standard-format containers.
				ret = ictx->on_std_container_start_fn(c, ictx);
				if(!ret) goto done;
			}
		}
		else { // ictx->is_raw_container
			contents_dpos = chunkctx.dpos;
			contents_dlen = chunkctx.dlen;
		}

		ret = do_iff_chunk_sequence(c, ictx, contents_dpos, contents_dlen, level+1);
		if(!ret) {
			retval = 0;
			goto done;
		}

		if(ictx->on_container_end_fn) {
			// Call for all containers (not just standard-format containers).

			// TODO: Decide exactly what ictx->* fields to set here.
			ictx->level = level;

			ictx->chunkctx = NULL;
			ret = ictx->on_container_end_fn(c, ictx);
			if(!ret) {
				retval = 0;
				goto done;
			}
		}
	}
	else if(!ictx->handled) {
		de_fmtutil_default_iff_chunk_handler(c, ictx);
	}

done:
	fourcc_clear(&ictx->curr_container_fmt4cc);
	fourcc_clear(&ictx->curr_container_contentstype4cc);

	de_dbg_indent_restore(c, saved_indent_level);
	return retval;
}