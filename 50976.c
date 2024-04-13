static int ffs_ep0_release(struct inode *inode, struct file *file)
{
	struct ffs_data *ffs = file->private_data;

	ENTER();

	ffs_data_closed(ffs);

	return 0;
}
