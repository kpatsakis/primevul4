static ssize_t read_iter_null(struct kiocb *iocb, struct iov_iter *to)
{
	return 0;
}
