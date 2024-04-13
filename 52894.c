static void file_change_deleteall(struct branch *b)
{
	release_tree_content_recursive(b->branch_tree.tree);
	hashclr(b->branch_tree.versions[0].sha1);
	hashclr(b->branch_tree.versions[1].sha1);
	load_tree(&b->branch_tree);
	b->num_notes = 0;
}
