static int btrfs_find_actor(struct inode *inode, void *opaque)
{
	struct btrfs_iget_args *args = opaque;
	return args->location->objectid == BTRFS_I(inode)->location.objectid &&
		args->root == BTRFS_I(inode)->root;
}
