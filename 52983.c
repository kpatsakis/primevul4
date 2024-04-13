static int rev_same_tree_as_empty(struct rev_info *revs, struct commit *commit)
{
	int retval;
	struct tree *t1 = commit->tree;

	if (!t1)
		return 0;

	tree_difference = REV_TREE_SAME;
	DIFF_OPT_CLR(&revs->pruning, HAS_CHANGES);
	retval = diff_tree_sha1(NULL, t1->object.sha1, "", &revs->pruning);

	return retval >= 0 && (tree_difference == REV_TREE_SAME);
}
