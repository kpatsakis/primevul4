static int btrfs_insert_inode_locked(struct inode *inode)
{
	struct btrfs_iget_args args;
	args.location = &BTRFS_I(inode)->location;
	args.root = BTRFS_I(inode)->root;

	return insert_inode_locked4(inode,
		   btrfs_inode_hash(inode->i_ino, BTRFS_I(inode)->root),
		   btrfs_find_actor, &args);
}
