static void parse_new_commit(const char *arg)
{
	static struct strbuf msg = STRBUF_INIT;
	struct branch *b;
	char *author = NULL;
	char *committer = NULL;
	struct hash_list *merge_list = NULL;
	unsigned int merge_count;
	unsigned char prev_fanout, new_fanout;
	const char *v;

	b = lookup_branch(arg);
	if (!b)
		b = new_branch(arg);

	read_next_command();
	parse_mark();
	if (skip_prefix(command_buf.buf, "author ", &v)) {
		author = parse_ident(v);
		read_next_command();
	}
	if (skip_prefix(command_buf.buf, "committer ", &v)) {
		committer = parse_ident(v);
		read_next_command();
	}
	if (!committer)
		die("Expected committer but didn't get one");
	parse_data(&msg, 0, NULL);
	read_next_command();
	parse_from(b);
	merge_list = parse_merge(&merge_count);

	/* ensure the branch is active/loaded */
	if (!b->branch_tree.tree || !max_active_branches) {
		unload_one_branch();
		load_branch(b);
	}

	prev_fanout = convert_num_notes_to_fanout(b->num_notes);

	/* file_change* */
	while (command_buf.len > 0) {
		if (skip_prefix(command_buf.buf, "M ", &v))
			file_change_m(v, b);
		else if (skip_prefix(command_buf.buf, "D ", &v))
			file_change_d(v, b);
		else if (skip_prefix(command_buf.buf, "R ", &v))
			file_change_cr(v, b, 1);
		else if (skip_prefix(command_buf.buf, "C ", &v))
			file_change_cr(v, b, 0);
		else if (skip_prefix(command_buf.buf, "N ", &v))
			note_change_n(v, b, &prev_fanout);
		else if (!strcmp("deleteall", command_buf.buf))
			file_change_deleteall(b);
		else if (skip_prefix(command_buf.buf, "ls ", &v))
			parse_ls(v, b);
		else {
			unread_command_buf = 1;
			break;
		}
		if (read_next_command() == EOF)
			break;
	}

	new_fanout = convert_num_notes_to_fanout(b->num_notes);
	if (new_fanout != prev_fanout)
		b->num_notes = change_note_fanout(&b->branch_tree, new_fanout);

	/* build the tree and the commit */
	store_tree(&b->branch_tree);
	hashcpy(b->branch_tree.versions[0].sha1,
		b->branch_tree.versions[1].sha1);

	strbuf_reset(&new_data);
	strbuf_addf(&new_data, "tree %s\n",
		sha1_to_hex(b->branch_tree.versions[1].sha1));
	if (!is_null_sha1(b->sha1))
		strbuf_addf(&new_data, "parent %s\n", sha1_to_hex(b->sha1));
	while (merge_list) {
		struct hash_list *next = merge_list->next;
		strbuf_addf(&new_data, "parent %s\n", sha1_to_hex(merge_list->sha1));
		free(merge_list);
		merge_list = next;
	}
	strbuf_addf(&new_data,
		"author %s\n"
		"committer %s\n"
		"\n",
		author ? author : committer, committer);
	strbuf_addbuf(&new_data, &msg);
	free(author);
	free(committer);

	if (!store_object(OBJ_COMMIT, &new_data, NULL, b->sha1, next_mark))
		b->pack_id = pack_id;
	b->last_commit = object_count_by_type[OBJ_COMMIT];
}
