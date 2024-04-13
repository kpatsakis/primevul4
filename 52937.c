static struct hash_list *parse_merge(unsigned int *count)
{
	struct hash_list *list = NULL, **tail = &list, *n;
	const char *from;
	struct branch *s;

	*count = 0;
	while (skip_prefix(command_buf.buf, "merge ", &from)) {
		n = xmalloc(sizeof(*n));
		s = lookup_branch(from);
		if (s)
			hashcpy(n->sha1, s->sha1);
		else if (*from == ':') {
			uintmax_t idnum = parse_mark_ref_eol(from);
			struct object_entry *oe = find_mark(idnum);
			if (oe->type != OBJ_COMMIT)
				die("Mark :%" PRIuMAX " not a commit", idnum);
			hashcpy(n->sha1, oe->idx.sha1);
		} else if (!get_sha1(from, n->sha1)) {
			unsigned long size;
			char *buf = read_object_with_reference(n->sha1,
				commit_type, &size, n->sha1);
			if (!buf || size < 46)
				die("Not a valid commit: %s", from);
			free(buf);
		} else
			die("Invalid ref name or SHA1 expression: %s", from);

		n->next = NULL;
		*tail = n;
		tail = &n->next;

		(*count)++;
		read_next_command();
	}
	return list;
}
