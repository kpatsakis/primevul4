static void parse_new_tag(const char *arg)
{
	static struct strbuf msg = STRBUF_INIT;
	const char *from;
	char *tagger;
	struct branch *s;
	struct tag *t;
	uintmax_t from_mark = 0;
	unsigned char sha1[20];
	enum object_type type;
	const char *v;

	t = pool_alloc(sizeof(struct tag));
	memset(t, 0, sizeof(struct tag));
	t->name = pool_strdup(arg);
	if (last_tag)
		last_tag->next_tag = t;
	else
		first_tag = t;
	last_tag = t;
	read_next_command();

	/* from ... */
	if (!skip_prefix(command_buf.buf, "from ", &from))
		die("Expected from command, got %s", command_buf.buf);
	s = lookup_branch(from);
	if (s) {
		if (is_null_sha1(s->sha1))
			die("Can't tag an empty branch.");
		hashcpy(sha1, s->sha1);
		type = OBJ_COMMIT;
	} else if (*from == ':') {
		struct object_entry *oe;
		from_mark = parse_mark_ref_eol(from);
		oe = find_mark(from_mark);
		type = oe->type;
		hashcpy(sha1, oe->idx.sha1);
	} else if (!get_sha1(from, sha1)) {
		struct object_entry *oe = find_object(sha1);
		if (!oe) {
			type = sha1_object_info(sha1, NULL);
			if (type < 0)
				die("Not a valid object: %s", from);
		} else
			type = oe->type;
	} else
		die("Invalid ref name or SHA1 expression: %s", from);
	read_next_command();

	/* tagger ... */
	if (skip_prefix(command_buf.buf, "tagger ", &v)) {
		tagger = parse_ident(v);
		read_next_command();
	} else
		tagger = NULL;

	/* tag payload/message */
	parse_data(&msg, 0, NULL);

	/* build the tag object */
	strbuf_reset(&new_data);

	strbuf_addf(&new_data,
		    "object %s\n"
		    "type %s\n"
		    "tag %s\n",
		    sha1_to_hex(sha1), typename(type), t->name);
	if (tagger)
		strbuf_addf(&new_data,
			    "tagger %s\n", tagger);
	strbuf_addch(&new_data, '\n');
	strbuf_addbuf(&new_data, &msg);
	free(tagger);

	if (store_object(OBJ_TAG, &new_data, NULL, t->sha1, 0))
		t->pack_id = MAX_PACK_ID;
	else
		t->pack_id = pack_id;
}
