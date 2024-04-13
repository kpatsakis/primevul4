static ssize_t __fuse_direct_read(struct fuse_io_priv *io,
				  struct iov_iter *iter,
				  loff_t *ppos)
{
	ssize_t res;
	struct file *file = io->file;
	struct inode *inode = file_inode(file);

	if (is_bad_inode(inode))
		return -EIO;

	res = fuse_direct_io(io, iter, ppos, 0);

	fuse_invalidate_attr(inode);

	return res;
}
