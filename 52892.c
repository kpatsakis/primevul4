static void file_change_cr(const char *s, struct branch *b, int rename)
{
	const char *d;
	static struct strbuf s_uq = STRBUF_INIT;
	static struct strbuf d_uq = STRBUF_INIT;
	const char *endp;
	struct tree_entry leaf;

	strbuf_reset(&s_uq);
	if (!unquote_c_style(&s_uq, s, &endp)) {
		if (*endp != ' ')
			die("Missing space after source: %s", command_buf.buf);
	} else {
		endp = strchr(s, ' ');
		if (!endp)
			die("Missing space after source: %s", command_buf.buf);
		strbuf_add(&s_uq, s, endp - s);
	}
	s = s_uq.buf;

	endp++;
	if (!*endp)
		die("Missing dest: %s", command_buf.buf);

	d = endp;
	strbuf_reset(&d_uq);
	if (!unquote_c_style(&d_uq, d, &endp)) {
		if (*endp)
			die("Garbage after dest in: %s", command_buf.buf);
		d = d_uq.buf;
	}

	memset(&leaf, 0, sizeof(leaf));
	if (rename)
		tree_content_remove(&b->branch_tree, s, &leaf, 1);
	else
		tree_content_get(&b->branch_tree, s, &leaf, 1);
	if (!leaf.versions[1].mode)
		die("Path %s not in branch", s);
	if (!*d) {	/* C "path/to/subdir" "" */
		tree_content_replace(&b->branch_tree,
			leaf.versions[1].sha1,
			leaf.versions[1].mode,
			leaf.tree);
		return;
	}
	tree_content_set(&b->branch_tree, d,
		leaf.versions[1].sha1,
		leaf.versions[1].mode,
		leaf.tree);
}
