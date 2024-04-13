static ssize_t ubifs_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	int err = update_mctime(file_inode(iocb->ki_filp));
	if (err)
		return err;

	return generic_file_write_iter(iocb, from);
}
