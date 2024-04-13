static void parse_get_mark(const char *p)
{
	struct object_entry *oe = oe;
	char output[42];

	/* get-mark SP <object> LF */
	if (*p != ':')
		die("Not a mark: %s", p);

	oe = find_mark(parse_mark_ref_eol(p));
	if (!oe)
		die("Unknown mark: %s", command_buf.buf);

	snprintf(output, sizeof(output), "%s\n", sha1_to_hex(oe->idx.sha1));
	cat_blob_write(output, 41);
}
