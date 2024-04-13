static ssize_t cuse_read_iter(struct kiocb *kiocb, struct iov_iter *to)
{
	struct fuse_io_priv io = { .async = 0, .file = kiocb->ki_filp };
	loff_t pos = 0;

	return fuse_direct_io(&io, to, &pos, FUSE_DIO_CUSE);
}
