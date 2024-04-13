static ssize_t fuse_direct_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct fuse_io_priv io = { .async = 0, .file = file };
	ssize_t res;

	if (is_bad_inode(inode))
		return -EIO;

	/* Don't allow parallel writes to the same file */
	mutex_lock(&inode->i_mutex);
	res = generic_write_checks(iocb, from);
	if (res > 0)
		res = fuse_direct_io(&io, from, &iocb->ki_pos, FUSE_DIO_WRITE);
	fuse_invalidate_attr(inode);
	if (res > 0)
		fuse_write_update_size(inode, iocb->ki_pos);
	mutex_unlock(&inode->i_mutex);

	return res;
}
