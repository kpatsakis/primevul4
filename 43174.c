static ssize_t udf_direct_IO(int rw, struct kiocb *iocb,
			     struct iov_iter *iter,
			     loff_t offset)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	size_t count = iov_iter_count(iter);
	ssize_t ret;

	ret = blockdev_direct_IO(rw, iocb, inode, iter, offset, udf_get_block);
	if (unlikely(ret < 0 && (rw & WRITE)))
		udf_write_failed(mapping, offset + count);
	return ret;
}
