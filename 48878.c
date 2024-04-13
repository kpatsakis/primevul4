SYSCALL_DEFINE4(vmsplice, int, fd, const struct iovec __user *, iov,
		unsigned long, nr_segs, unsigned int, flags)
{
	struct fd f;
	long error;

	if (unlikely(nr_segs > UIO_MAXIOV))
		return -EINVAL;
	else if (unlikely(!nr_segs))
		return 0;

	error = -EBADF;
	f = fdget(fd);
	if (f.file) {
		if (f.file->f_mode & FMODE_WRITE)
			error = vmsplice_to_pipe(f.file, iov, nr_segs, flags);
		else if (f.file->f_mode & FMODE_READ)
			error = vmsplice_to_user(f.file, iov, nr_segs, flags);

		fdput(f);
	}

	return error;
}
