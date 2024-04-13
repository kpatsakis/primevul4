fuse_direct_IO(struct kiocb *iocb, struct iov_iter *iter, loff_t offset)
{
	DECLARE_COMPLETION_ONSTACK(wait);
	ssize_t ret = 0;
	struct file *file = iocb->ki_filp;
	struct fuse_file *ff = file->private_data;
	bool async_dio = ff->fc->async_dio;
	loff_t pos = 0;
	struct inode *inode;
	loff_t i_size;
	size_t count = iov_iter_count(iter);
	struct fuse_io_priv *io;

	pos = offset;
	inode = file->f_mapping->host;
	i_size = i_size_read(inode);

	if ((iov_iter_rw(iter) == READ) && (offset > i_size))
		return 0;

	/* optimization for short read */
	if (async_dio && iov_iter_rw(iter) != WRITE && offset + count > i_size) {
		if (offset >= i_size)
			return 0;
		iov_iter_truncate(iter, fuse_round_up(i_size - offset));
		count = iov_iter_count(iter);
	}

	io = kmalloc(sizeof(struct fuse_io_priv), GFP_KERNEL);
	if (!io)
		return -ENOMEM;
	spin_lock_init(&io->lock);
	io->reqs = 1;
	io->bytes = -1;
	io->size = 0;
	io->offset = offset;
	io->write = (iov_iter_rw(iter) == WRITE);
	io->err = 0;
	io->file = file;
	/*
	 * By default, we want to optimize all I/Os with async request
	 * submission to the client filesystem if supported.
	 */
	io->async = async_dio;
	io->iocb = iocb;

	/*
	 * We cannot asynchronously extend the size of a file. We have no method
	 * to wait on real async I/O requests, so we must submit this request
	 * synchronously.
	 */
	if (!is_sync_kiocb(iocb) && (offset + count > i_size) &&
	    iov_iter_rw(iter) == WRITE)
		io->async = false;

	if (io->async && is_sync_kiocb(iocb))
		io->done = &wait;

	if (iov_iter_rw(iter) == WRITE) {
		ret = fuse_direct_io(io, iter, &pos, FUSE_DIO_WRITE);
		fuse_invalidate_attr(inode);
	} else {
		ret = __fuse_direct_read(io, iter, &pos);
	}

	if (io->async) {
		fuse_aio_complete(io, ret < 0 ? ret : 0, -1);

		/* we have a non-extending, async request, so return */
		if (!is_sync_kiocb(iocb))
			return -EIOCBQUEUED;

		wait_for_completion(&wait);
		ret = fuse_get_res_by_io(io);
	}

	kfree(io);

	if (iov_iter_rw(iter) == WRITE) {
		if (ret > 0)
			fuse_write_update_size(inode, pos);
		else if (ret < 0 && offset + count > i_size)
			fuse_do_truncate(file);
	}

	return ret;
}
