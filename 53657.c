static ssize_t fuse_direct_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct fuse_io_priv io = { .async = 0, .file = iocb->ki_filp };
	return __fuse_direct_read(&io, to, &iocb->ki_pos);
}
