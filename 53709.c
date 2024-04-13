int fuse_write_inode(struct inode *inode, struct writeback_control *wbc)
{
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct fuse_file *ff;
	int err;

	ff = __fuse_write_file_get(fc, fi);
	err = fuse_flush_times(inode, ff);
	if (ff)
		fuse_file_put(ff, 0);

	return err;
}
