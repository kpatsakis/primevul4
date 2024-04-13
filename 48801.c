ssize_t blkdev_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct blk_plug plug;
	ssize_t ret;

	blk_start_plug(&plug);
	ret = __generic_file_write_iter(iocb, from);
	if (ret > 0) {
		ssize_t err;
		err = generic_write_sync(file, iocb->ki_pos - ret, ret);
		if (err < 0)
			ret = err;
	}
	blk_finish_plug(&plug);
	return ret;
}
