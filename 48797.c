static ssize_t blkdev_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *file = iocb->ki_filp;
	struct inode *bd_inode = file->f_mapping->host;
	loff_t size = i_size_read(bd_inode);
	loff_t pos = iocb->ki_pos;

	if (pos >= size)
		return 0;

	size -= pos;
	iov_iter_truncate(to, size);
	return generic_file_read_iter(iocb, to);
}
