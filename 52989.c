static void try_to_simplify_commit(struct rev_info *revs, struct commit *commit)
{
	struct commit_list **pp, *parent;
	struct treesame_state *ts = NULL;
	int relevant_change = 0, irrelevant_change = 0;
	int relevant_parents, nth_parent;

	/*
	 * If we don't do pruning, everything is interesting
	 */
	if (!revs->prune)
		return;

	if (!commit->tree)
		return;

	if (!commit->parents) {
		if (rev_same_tree_as_empty(revs, commit))
			commit->object.flags |= TREESAME;
		return;
	}

	/*
	 * Normal non-merge commit? If we don't want to make the
	 * history dense, we consider it always to be a change..
	 */
	if (!revs->dense && !commit->parents->next)
		return;

	for (pp = &commit->parents, nth_parent = 0, relevant_parents = 0;
	     (parent = *pp) != NULL;
	     pp = &parent->next, nth_parent++) {
		struct commit *p = parent->item;
		if (relevant_commit(p))
			relevant_parents++;

		if (nth_parent == 1) {
			/*
			 * This our second loop iteration - so we now know
			 * we're dealing with a merge.
			 *
			 * Do not compare with later parents when we care only about
			 * the first parent chain, in order to avoid derailing the
			 * traversal to follow a side branch that brought everything
			 * in the path we are limited to by the pathspec.
			 */
			if (revs->first_parent_only)
				break;
			/*
			 * If this will remain a potentially-simplifiable
			 * merge, remember per-parent treesame if needed.
			 * Initialise the array with the comparison from our
			 * first iteration.
			 */
			if (revs->treesame.name &&
			    !revs->simplify_history &&
			    !(commit->object.flags & UNINTERESTING)) {
				ts = initialise_treesame(revs, commit);
				if (!(irrelevant_change || relevant_change))
					ts->treesame[0] = 1;
			}
		}
		if (parse_commit(p) < 0)
			die("cannot simplify commit %s (because of %s)",
			    sha1_to_hex(commit->object.sha1),
			    sha1_to_hex(p->object.sha1));
		switch (rev_compare_tree(revs, p, commit)) {
		case REV_TREE_SAME:
			if (!revs->simplify_history || !relevant_commit(p)) {
				/* Even if a merge with an uninteresting
				 * side branch brought the entire change
				 * we are interested in, we do not want
				 * to lose the other branches of this
				 * merge, so we just keep going.
				 */
				if (ts)
					ts->treesame[nth_parent] = 1;
				continue;
			}
			parent->next = NULL;
			commit->parents = parent;
			commit->object.flags |= TREESAME;
			return;

		case REV_TREE_NEW:
			if (revs->remove_empty_trees &&
			    rev_same_tree_as_empty(revs, p)) {
				/* We are adding all the specified
				 * paths from this parent, so the
				 * history beyond this parent is not
				 * interesting.  Remove its parents
				 * (they are grandparents for us).
				 * IOW, we pretend this parent is a
				 * "root" commit.
				 */
				if (parse_commit(p) < 0)
					die("cannot simplify commit %s (invalid %s)",
					    sha1_to_hex(commit->object.sha1),
					    sha1_to_hex(p->object.sha1));
				p->parents = NULL;
			}
		/* fallthrough */
		case REV_TREE_OLD:
		case REV_TREE_DIFFERENT:
			if (relevant_commit(p))
				relevant_change = 1;
			else
				irrelevant_change = 1;
			continue;
		}
		die("bad tree compare for commit %s", sha1_to_hex(commit->object.sha1));
	}

	/*
	 * TREESAME is straightforward for single-parent commits. For merge
	 * commits, it is most useful to define it so that "irrelevant"
	 * parents cannot make us !TREESAME - if we have any relevant
	 * parents, then we only consider TREESAMEness with respect to them,
	 * allowing irrelevant merges from uninteresting branches to be
	 * simplified away. Only if we have only irrelevant parents do we
	 * base TREESAME on them. Note that this logic is replicated in
	 * update_treesame, which should be kept in sync.
	 */
	if (relevant_parents ? !relevant_change : !irrelevant_change)
		commit->object.flags |= TREESAME;
}
