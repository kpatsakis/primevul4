static void file_change_d(const char *p, struct branch *b)
{
	static struct strbuf uq = STRBUF_INIT;
	const char *endp;

	strbuf_reset(&uq);
	if (!unquote_c_style(&uq, p, &endp)) {
		if (*endp)
			die("Garbage after path in: %s", command_buf.buf);
		p = uq.buf;
	}
	tree_content_remove(&b->branch_tree, p, NULL, 1);
}
