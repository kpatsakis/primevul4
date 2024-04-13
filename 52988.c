static void track_linear(struct rev_info *revs, struct commit *commit)
{
	if (revs->track_first_time) {
		revs->linear = 1;
		revs->track_first_time = 0;
	} else {
		struct commit_list *p;
		for (p = revs->previous_parents; p; p = p->next)
			if (p->item == NULL || /* first commit */
			    !hashcmp(p->item->object.sha1, commit->object.sha1))
				break;
		revs->linear = p != NULL;
	}
	if (revs->reverse) {
		if (revs->linear)
			commit->object.flags |= TRACK_LINEAR;
	}
	free_commit_list(revs->previous_parents);
	revs->previous_parents = copy_commit_list(commit->parents);
}
