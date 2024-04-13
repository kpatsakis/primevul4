static int do_box(deark *c, struct de_boxesctx *bctx, i64 pos, i64 len,
	int level, i64 *pbytes_consumed)
{
	i64 size32, size64;
	i64 header_len; // Not including UUIDs
	i64 payload_len; // Including UUIDs
	i64 total_len;
	struct de_fourcc box4cc;
	char uuid_string[50];
	int ret;
	int retval = 0;
	struct de_boxdata *parentbox;
	struct de_boxdata *curbox;

	parentbox = bctx->curbox;
	bctx->curbox = de_malloc(c, sizeof(struct de_boxdata));
	curbox = bctx->curbox;
	curbox->parent = parentbox;

	if(len<8) {
		de_dbg(c, "(ignoring %d extra bytes at %"I64_FMT")", (int)len, pos);
		goto done;
	}

	size32 = dbuf_getu32be(bctx->f, pos);
	dbuf_read_fourcc(bctx->f, pos+4, &box4cc, 4, 0x0);
	curbox->boxtype = box4cc.id;

	if(size32>=8) {
		header_len = 8;
		payload_len = size32-8;
	}
	else if(size32==0) {
		header_len = 8;
		payload_len = len-8;
	}
	else if(size32==1) {
		if(len<16) {
			de_dbg(c, "(ignoring %d extra bytes at %"I64_FMT")", (int)len, pos);
			goto done;
		}
		header_len = 16;
		size64 = dbuf_geti64be(bctx->f, pos+8);
		if(size64<16) goto done;
		payload_len = size64-16;
	}
	else {
		de_err(c, "Invalid or unsupported box format");
		goto done;
	}

	total_len = header_len + payload_len;

	if(curbox->boxtype==DE_BOX_uuid && payload_len>=16) {
		curbox->is_uuid = 1;
		dbuf_read(bctx->f, curbox->uuid, pos+header_len, 16);
	}

	curbox->level = level;
	curbox->box_pos = pos;
	curbox->box_len = total_len;
	curbox->payload_pos = pos+header_len;
	curbox->payload_len = payload_len;
	if(curbox->is_uuid) {
		curbox->payload_pos += 16;
		curbox->payload_len -= 16;
	}

	if(bctx->identify_box_fn) {
		bctx->identify_box_fn(c, bctx);
	}

	if(c->debug_level>0) {
		char name_str[80];

		if(curbox->box_name) {
			de_snprintf(name_str, sizeof(name_str), " (%s)", curbox->box_name);
		}
		else {
			name_str[0] = '\0';
		}

		if(curbox->is_uuid) {
			fmtutil_render_uuid(c, curbox->uuid, uuid_string, sizeof(uuid_string));
			de_dbg(c, "box '%s'{%s}%s at %"I64_FMT", len=%"I64_FMT,
				box4cc.id_dbgstr, uuid_string, name_str,
				pos, total_len);
		}
		else {
			de_dbg(c, "box '%s'%s at %"I64_FMT", len=%"I64_FMT", dlen=%"I64_FMT,
				box4cc.id_dbgstr, name_str, pos,
				total_len, payload_len);
		}
	}

	if(total_len > len) {
		de_err(c, "Invalid oversized box, or unexpected end of file "
			"(box at %"I64_FMT" ends at %"I64_FMT", "
			"parent ends at %"I64_FMT")",
			pos, pos+total_len, pos+len);
		goto done;
	}

	de_dbg_indent(c, 1);
	ret = bctx->handle_box_fn(c, bctx);
	de_dbg_indent(c, -1);
	if(!ret) goto done;

	if(curbox->is_superbox) {
		i64 children_pos, children_len;
		i64 max_nchildren;

		de_dbg_indent(c, 1);
		children_pos = curbox->payload_pos + curbox->extra_bytes_before_children;
		children_len = curbox->payload_len - curbox->extra_bytes_before_children;
		max_nchildren = (curbox->num_children_is_known) ? curbox->num_children : -1;
		do_box_sequence(c, bctx, children_pos, children_len, max_nchildren, level+1);
		de_dbg_indent(c, -1);
	}

	*pbytes_consumed = total_len;
	retval = 1;

done:
	de_free(c, bctx->curbox);
	bctx->curbox = parentbox; // Restore the curbox pointer
	return retval;
}