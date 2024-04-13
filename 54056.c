static ssize_t cuse_write_iter(struct kiocb *kiocb, struct iov_iter *from)
{
	struct fuse_io_priv io = { .async = 0, .file = kiocb->ki_filp };
	loff_t pos = 0;
	/*
	 * No locking or generic_write_checks(), the server is
	 * responsible for locking and sanity checks.
	 */
	return fuse_direct_io(&io, from, &pos,
			      FUSE_DIO_WRITE | FUSE_DIO_CUSE);
}
