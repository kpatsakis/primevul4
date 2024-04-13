static int ffs_sb_fill(struct super_block *sb, void *_data, int silent)
{
	struct ffs_sb_fill_data *data = _data;
	struct inode	*inode;
	struct ffs_data	*ffs = data->ffs_data;

	ENTER();

	ffs->sb              = sb;
	data->ffs_data       = NULL;
	sb->s_fs_info        = ffs;
	sb->s_blocksize      = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic          = FUNCTIONFS_MAGIC;
	sb->s_op             = &ffs_sb_operations;
	sb->s_time_gran      = 1;

	/* Root inode */
	data->perms.mode = data->root_mode;
	inode = ffs_sb_make_inode(sb, NULL,
				  &simple_dir_operations,
				  &simple_dir_inode_operations,
				  &data->perms);
	sb->s_root = d_make_root(inode);
	if (unlikely(!sb->s_root))
		return -ENOMEM;

	/* EP0 file */
	if (unlikely(!ffs_sb_create_file(sb, "ep0", ffs,
					 &ffs_ep0_operations)))
		return -ENOMEM;

	return 0;
}
