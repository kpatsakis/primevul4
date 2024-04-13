static void parse_cat_blob(const char *p)
{
	struct object_entry *oe = oe;
	unsigned char sha1[20];

	/* cat-blob SP <object> LF */
	if (*p == ':') {
		oe = find_mark(parse_mark_ref_eol(p));
		if (!oe)
			die("Unknown mark: %s", command_buf.buf);
		hashcpy(sha1, oe->idx.sha1);
	} else {
		if (get_sha1_hex(p, sha1))
			die("Invalid dataref: %s", command_buf.buf);
		if (p[40])
			die("Garbage after SHA1: %s", command_buf.buf);
		oe = find_object(sha1);
	}

	cat_blob(oe, sha1);
}
