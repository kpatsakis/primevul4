static struct object_entry *parse_treeish_dataref(const char **p)
{
	unsigned char sha1[20];
	struct object_entry *e;

	if (**p == ':') {	/* <mark> */
		e = find_mark(parse_mark_ref_space(p));
		if (!e)
			die("Unknown mark: %s", command_buf.buf);
		hashcpy(sha1, e->idx.sha1);
	} else {	/* <sha1> */
		if (get_sha1_hex(*p, sha1))
			die("Invalid dataref: %s", command_buf.buf);
		e = find_object(sha1);
		*p += 40;
		if (*(*p)++ != ' ')
			die("Missing space after tree-ish: %s", command_buf.buf);
	}

	while (!e || e->type != OBJ_TREE)
		e = dereference(e, sha1);
	return e;
}
