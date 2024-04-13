static int btrfs_init_locked_inode(struct inode *inode, void *p)
{
	struct btrfs_iget_args *args = p;
	inode->i_ino = args->location->objectid;
	memcpy(&BTRFS_I(inode)->location, args->location,
	       sizeof(*args->location));
	BTRFS_I(inode)->root = args->root;
	return 0;
}
