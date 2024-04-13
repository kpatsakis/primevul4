static inline int inode_need_compress(struct inode *inode)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;

	/* force compress */
	if (btrfs_test_opt(root, FORCE_COMPRESS))
		return 1;
	/* bad compression ratios */
	if (BTRFS_I(inode)->flags & BTRFS_INODE_NOCOMPRESS)
		return 0;
	if (btrfs_test_opt(root, COMPRESS) ||
	    BTRFS_I(inode)->flags & BTRFS_INODE_COMPRESS ||
	    BTRFS_I(inode)->force_compress)
		return 1;
	return 0;
}
