static ssize_t tun_chr_aio_write(struct kiocb *iocb, const struct iovec *iv,
			      unsigned long count, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct tun_struct *tun = tun_get(file);
	ssize_t result;

	if (!tun)
		return -EBADFD;

	tun_debug(KERN_INFO, tun, "tun_chr_write %ld\n", count);

	result = tun_get_user(tun, NULL, iv, iov_length(iv, count), count,
			      file->f_flags & O_NONBLOCK);

	tun_put(tun);
	return result;
}
