static void btrfs_del_delalloc_inode(struct btrfs_root *root,
				     struct inode *inode)
{
	spin_lock(&root->delalloc_lock);
	if (!list_empty(&BTRFS_I(inode)->delalloc_inodes)) {
		list_del_init(&BTRFS_I(inode)->delalloc_inodes);
		clear_bit(BTRFS_INODE_IN_DELALLOC_LIST,
			  &BTRFS_I(inode)->runtime_flags);
		root->nr_delalloc_inodes--;
		if (!root->nr_delalloc_inodes) {
			spin_lock(&root->fs_info->delalloc_root_lock);
			BUG_ON(list_empty(&root->delalloc_root));
			list_del_init(&root->delalloc_root);
			spin_unlock(&root->fs_info->delalloc_root_lock);
		}
	}
	spin_unlock(&root->delalloc_lock);
}
