static void btrfs_add_delalloc_inodes(struct btrfs_root *root,
				      struct inode *inode)
{
	spin_lock(&root->delalloc_lock);
	if (list_empty(&BTRFS_I(inode)->delalloc_inodes)) {
		list_add_tail(&BTRFS_I(inode)->delalloc_inodes,
			      &root->delalloc_inodes);
		set_bit(BTRFS_INODE_IN_DELALLOC_LIST,
			&BTRFS_I(inode)->runtime_flags);
		root->nr_delalloc_inodes++;
		if (root->nr_delalloc_inodes == 1) {
			spin_lock(&root->fs_info->delalloc_root_lock);
			BUG_ON(!list_empty(&root->delalloc_root));
			list_add_tail(&root->delalloc_root,
				      &root->fs_info->delalloc_roots);
			spin_unlock(&root->fs_info->delalloc_root_lock);
		}
	}
	spin_unlock(&root->delalloc_lock);
}
