static int parse_from(struct branch *b)
{
	const char *from;
	struct branch *s;
	unsigned char sha1[20];

	if (!skip_prefix(command_buf.buf, "from ", &from))
		return 0;

	hashcpy(sha1, b->branch_tree.versions[1].sha1);

	s = lookup_branch(from);
	if (b == s)
		die("Can't create a branch from itself: %s", b->name);
	else if (s) {
		unsigned char *t = s->branch_tree.versions[1].sha1;
		hashcpy(b->sha1, s->sha1);
		hashcpy(b->branch_tree.versions[0].sha1, t);
		hashcpy(b->branch_tree.versions[1].sha1, t);
	} else if (*from == ':') {
		uintmax_t idnum = parse_mark_ref_eol(from);
		struct object_entry *oe = find_mark(idnum);
		if (oe->type != OBJ_COMMIT)
			die("Mark :%" PRIuMAX " not a commit", idnum);
		if (hashcmp(b->sha1, oe->idx.sha1)) {
			hashcpy(b->sha1, oe->idx.sha1);
			if (oe->pack_id != MAX_PACK_ID) {
				unsigned long size;
				char *buf = gfi_unpack_entry(oe, &size);
				parse_from_commit(b, buf, size);
				free(buf);
			} else
				parse_from_existing(b);
		}
	} else if (!get_sha1(from, b->sha1)) {
		parse_from_existing(b);
		if (is_null_sha1(b->sha1))
			b->delete = 1;
	}
	else
		die("Invalid ref name or SHA1 expression: %s", from);

	if (b->branch_tree.tree && hashcmp(sha1, b->branch_tree.versions[1].sha1)) {
		release_tree_content_recursive(b->branch_tree.tree);
		b->branch_tree.tree = NULL;
	}

	read_next_command();
	return 1;
}
