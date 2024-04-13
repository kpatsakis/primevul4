static void relink_file_extents(struct new_sa_defrag_extent *new)
{
	struct btrfs_path *path;
	struct sa_defrag_extent_backref *backref;
	struct sa_defrag_extent_backref *prev = NULL;
	struct inode *inode;
	struct btrfs_root *root;
	struct rb_node *node;
	int ret;

	inode = new->inode;
	root = BTRFS_I(inode)->root;

	path = btrfs_alloc_path();
	if (!path)
		return;

	if (!record_extent_backrefs(path, new)) {
		btrfs_free_path(path);
		goto out;
	}
	btrfs_release_path(path);

	while (1) {
		node = rb_first(&new->root);
		if (!node)
			break;
		rb_erase(node, &new->root);

		backref = rb_entry(node, struct sa_defrag_extent_backref, node);

		ret = relink_extent_backref(path, prev, backref);
		WARN_ON(ret < 0);

		kfree(prev);

		if (ret == 1)
			prev = backref;
		else
			prev = NULL;
		cond_resched();
	}
	kfree(prev);

	btrfs_free_path(path);
out:
	free_sa_defrag_extent(new);

	atomic_dec(&root->fs_info->defrag_running);
	wake_up(&root->fs_info->transaction_wait);
}
