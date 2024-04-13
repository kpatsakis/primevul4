ssize_t cifs_user_readv(struct kiocb *iocb, const struct iovec *iov,
			       unsigned long nr_segs, loff_t pos)
{
	ssize_t read;

	read = cifs_iovec_read(iocb->ki_filp, iov, nr_segs, &pos);
	if (read > 0)
		iocb->ki_pos = pos;

	return read;
}
