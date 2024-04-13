static int add_parents_only(struct rev_info *revs, const char *arg_, int flags)
{
	unsigned char sha1[20];
	struct object *it;
	struct commit *commit;
	struct commit_list *parents;
	const char *arg = arg_;

	if (*arg == '^') {
		flags ^= UNINTERESTING | BOTTOM;
		arg++;
	}
	if (get_sha1_committish(arg, sha1))
		return 0;
	while (1) {
		it = get_reference(revs, arg, sha1, 0);
		if (!it && revs->ignore_missing)
			return 0;
		if (it->type != OBJ_TAG)
			break;
		if (!((struct tag*)it)->tagged)
			return 0;
		hashcpy(sha1, ((struct tag*)it)->tagged->sha1);
	}
	if (it->type != OBJ_COMMIT)
		return 0;
	commit = (struct commit *)it;
	for (parents = commit->parents; parents; parents = parents->next) {
		it = &parents->item->object;
		it->flags |= flags;
		add_rev_cmdline(revs, it, arg_, REV_CMD_PARENTS_ONLY, flags);
		add_pending_object(revs, it, arg);
	}
	return 1;
}
