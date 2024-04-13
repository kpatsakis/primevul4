ffs_epfile_release(struct inode *inode, struct file *file)
{
	struct ffs_epfile *epfile = inode->i_private;

	ENTER();

	ffs_data_closed(epfile->ffs);

	return 0;
}
