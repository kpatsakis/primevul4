static void parse_ls(const char *p, struct branch *b)
{
	struct tree_entry *root = NULL;
	struct tree_entry leaf = {NULL};

	/* ls SP (<tree-ish> SP)? <path> */
	if (*p == '"') {
		if (!b)
			die("Not in a commit: %s", command_buf.buf);
		root = &b->branch_tree;
	} else {
		struct object_entry *e = parse_treeish_dataref(&p);
		root = new_tree_entry();
		hashcpy(root->versions[1].sha1, e->idx.sha1);
		if (!is_null_sha1(root->versions[1].sha1))
			root->versions[1].mode = S_IFDIR;
		load_tree(root);
	}
	if (*p == '"') {
		static struct strbuf uq = STRBUF_INIT;
		const char *endp;
		strbuf_reset(&uq);
		if (unquote_c_style(&uq, p, &endp))
			die("Invalid path: %s", command_buf.buf);
		if (*endp)
			die("Garbage after path in: %s", command_buf.buf);
		p = uq.buf;
	}
	tree_content_get(root, p, &leaf, 1);
	/*
	 * A directory in preparation would have a sha1 of zero
	 * until it is saved.  Save, for simplicity.
	 */
	if (S_ISDIR(leaf.versions[1].mode))
		store_tree(&leaf);

	print_ls(leaf.versions[1].mode, leaf.versions[1].sha1, p);
	if (leaf.tree)
		release_tree_content_recursive(leaf.tree);
	if (!b || root != &b->branch_tree)
		release_tree_entry(root);
}
