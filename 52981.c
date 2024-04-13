void mark_tree_uninteresting(struct tree *tree)
{
	struct object *obj = &tree->object;

	if (!tree)
		return;
	if (obj->flags & UNINTERESTING)
		return;
	obj->flags |= UNINTERESTING;
	mark_tree_contents_uninteresting(tree);
}
