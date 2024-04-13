static int limit_list(struct rev_info *revs)
{
	int slop = SLOP;
	unsigned long date = ~0ul;
	struct commit_list *list = revs->commits;
	struct commit_list *newlist = NULL;
	struct commit_list **p = &newlist;
	struct commit_list *bottom = NULL;
	struct commit *interesting_cache = NULL;

	if (revs->ancestry_path) {
		bottom = collect_bottom_commits(list);
		if (!bottom)
			die("--ancestry-path given but there are no bottom commits");
	}

	while (list) {
		struct commit_list *entry = list;
		struct commit *commit = list->item;
		struct object *obj = &commit->object;
		show_early_output_fn_t show;

		list = list->next;
		free(entry);

		if (commit == interesting_cache)
			interesting_cache = NULL;

		if (revs->max_age != -1 && (commit->date < revs->max_age))
			obj->flags |= UNINTERESTING;
		if (add_parents_to_list(revs, commit, &list, NULL) < 0)
			return -1;
		if (obj->flags & UNINTERESTING) {
			mark_parents_uninteresting(commit);
			if (revs->show_all)
				p = &commit_list_insert(commit, p)->next;
			slop = still_interesting(list, date, slop, &interesting_cache);
			if (slop)
				continue;
			/* If showing all, add the whole pending list to the end */
			if (revs->show_all)
				*p = list;
			break;
		}
		if (revs->min_age != -1 && (commit->date > revs->min_age))
			continue;
		date = commit->date;
		p = &commit_list_insert(commit, p)->next;

		show = show_early_output;
		if (!show)
			continue;

		show(revs, newlist);
		show_early_output = NULL;
	}
	if (revs->cherry_pick || revs->cherry_mark)
		cherry_pick_list(newlist, revs);

	if (revs->left_only || revs->right_only)
		limit_left_right(newlist, revs);

	if (bottom) {
		limit_to_ancestry(bottom, newlist);
		free_commit_list(bottom);
	}

	/*
	 * Check if any commits have become TREESAME by some of their parents
	 * becoming UNINTERESTING.
	 */
	if (limiting_can_increase_treesame(revs))
		for (list = newlist; list; list = list->next) {
			struct commit *c = list->item;
			if (c->object.flags & (UNINTERESTING | TREESAME))
				continue;
			update_treesame(revs, c);
		}

	revs->commits = newlist;
	return 0;
}
