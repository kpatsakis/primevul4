static void file_change_m(const char *p, struct branch *b)
{
	static struct strbuf uq = STRBUF_INIT;
	const char *endp;
	struct object_entry *oe;
	unsigned char sha1[20];
	uint16_t mode, inline_data = 0;

	p = get_mode(p, &mode);
	if (!p)
		die("Corrupt mode: %s", command_buf.buf);
	switch (mode) {
	case 0644:
	case 0755:
		mode |= S_IFREG;
	case S_IFREG | 0644:
	case S_IFREG | 0755:
	case S_IFLNK:
	case S_IFDIR:
	case S_IFGITLINK:
		/* ok */
		break;
	default:
		die("Corrupt mode: %s", command_buf.buf);
	}

	if (*p == ':') {
		oe = find_mark(parse_mark_ref_space(&p));
		hashcpy(sha1, oe->idx.sha1);
	} else if (skip_prefix(p, "inline ", &p)) {
		inline_data = 1;
		oe = NULL; /* not used with inline_data, but makes gcc happy */
	} else {
		if (get_sha1_hex(p, sha1))
			die("Invalid dataref: %s", command_buf.buf);
		oe = find_object(sha1);
		p += 40;
		if (*p++ != ' ')
			die("Missing space after SHA1: %s", command_buf.buf);
	}

	strbuf_reset(&uq);
	if (!unquote_c_style(&uq, p, &endp)) {
		if (*endp)
			die("Garbage after path in: %s", command_buf.buf);
		p = uq.buf;
	}

	/* Git does not track empty, non-toplevel directories. */
	if (S_ISDIR(mode) && !hashcmp(sha1, EMPTY_TREE_SHA1_BIN) && *p) {
		tree_content_remove(&b->branch_tree, p, NULL, 0);
		return;
	}

	if (S_ISGITLINK(mode)) {
		if (inline_data)
			die("Git links cannot be specified 'inline': %s",
				command_buf.buf);
		else if (oe) {
			if (oe->type != OBJ_COMMIT)
				die("Not a commit (actually a %s): %s",
					typename(oe->type), command_buf.buf);
		}
		/*
		 * Accept the sha1 without checking; it expected to be in
		 * another repository.
		 */
	} else if (inline_data) {
		if (S_ISDIR(mode))
			die("Directories cannot be specified 'inline': %s",
				command_buf.buf);
		if (p != uq.buf) {
			strbuf_addstr(&uq, p);
			p = uq.buf;
		}
		read_next_command();
		parse_and_store_blob(&last_blob, sha1, 0);
	} else {
		enum object_type expected = S_ISDIR(mode) ?
						OBJ_TREE: OBJ_BLOB;
		enum object_type type = oe ? oe->type :
					sha1_object_info(sha1, NULL);
		if (type < 0)
			die("%s not found: %s",
					S_ISDIR(mode) ?  "Tree" : "Blob",
					command_buf.buf);
		if (type != expected)
			die("Not a %s (actually a %s): %s",
				typename(expected), typename(type),
				command_buf.buf);
	}

	if (!*p) {
		tree_content_replace(&b->branch_tree, sha1, mode, NULL);
		return;
	}
	tree_content_set(&b->branch_tree, p, sha1, mode, NULL);
}
