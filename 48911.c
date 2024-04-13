static long vmsplice_to_user(struct file *file, const struct iovec __user *uiov,
			     unsigned long nr_segs, unsigned int flags)
{
	struct pipe_inode_info *pipe;
	struct splice_desc sd;
	long ret;
	struct iovec iovstack[UIO_FASTIOV];
	struct iovec *iov = iovstack;
	struct iov_iter iter;
	ssize_t count = 0;

	pipe = get_pipe_info(file);
	if (!pipe)
		return -EBADF;

	ret = rw_copy_check_uvector(READ, uiov, nr_segs,
				    ARRAY_SIZE(iovstack), iovstack, &iov);
	if (ret <= 0)
		return ret;

	iov_iter_init(&iter, READ, iov, nr_segs, count);

	sd.len = 0;
	sd.total_len = count;
	sd.flags = flags;
	sd.u.data = &iter;
	sd.pos = 0;

	pipe_lock(pipe);
	ret = __splice_from_pipe(pipe, &sd, pipe_to_user);
	pipe_unlock(pipe);

	if (iov != iovstack)
		kfree(iov);

	return ret;
}
