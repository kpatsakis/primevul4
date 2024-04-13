void btrfs_test_inode_set_ops(struct inode *inode)
{
	BTRFS_I(inode)->io_tree.ops = &btrfs_extent_io_ops;
}
