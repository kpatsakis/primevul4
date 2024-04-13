static int cuse_release(struct inode *inode, struct file *file)
{
	struct fuse_file *ff = file->private_data;
	struct fuse_conn *fc = ff->fc;

	fuse_sync_release(ff, file->f_flags);
	fuse_conn_put(fc);

	return 0;
}
