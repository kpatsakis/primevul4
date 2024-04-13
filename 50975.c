static int ffs_ep0_open(struct inode *inode, struct file *file)
{
	struct ffs_data *ffs = inode->i_private;

	ENTER();

	if (unlikely(ffs->state == FFS_CLOSING))
		return -EBUSY;

	file->private_data = ffs;
	ffs_data_opened(ffs);

	return 0;
}
