void mark_parents_uninteresting(struct commit *commit)
{
	struct commit_list *parents = NULL, *l;

	for (l = commit->parents; l; l = l->next)
		commit_list_insert(l->item, &parents);

	while (parents) {
		struct commit *commit = parents->item;
		l = parents;
		parents = parents->next;
		free(l);

		while (commit) {
			/*
			 * A missing commit is ok iff its parent is marked
			 * uninteresting.
			 *
			 * We just mark such a thing parsed, so that when
			 * it is popped next time around, we won't be trying
			 * to parse it and get an error.
			 */
			if (!has_sha1_file(commit->object.sha1))
				commit->object.parsed = 1;

			if (commit->object.flags & UNINTERESTING)
				break;

			commit->object.flags |= UNINTERESTING;

			/*
			 * Normally we haven't parsed the parent
			 * yet, so we won't have a parent of a parent
			 * here. However, it may turn out that we've
			 * reached this commit some other way (where it
			 * wasn't uninteresting), in which case we need
			 * to mark its parents recursively too..
			 */
			if (!commit->parents)
				break;

			for (l = commit->parents->next; l; l = l->next)
				commit_list_insert(l->item, &parents);
			commit = commit->parents->item;
		}
	}
}
