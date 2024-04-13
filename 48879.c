SYSCALL_DEFINE4(tee, int, fdin, int, fdout, size_t, len, unsigned int, flags)
{
	struct fd in;
	int error;

	if (unlikely(!len))
		return 0;

	error = -EBADF;
	in = fdget(fdin);
	if (in.file) {
		if (in.file->f_mode & FMODE_READ) {
			struct fd out = fdget(fdout);
			if (out.file) {
				if (out.file->f_mode & FMODE_WRITE)
					error = do_tee(in.file, out.file,
							len, flags);
				fdput(out);
			}
		}
 		fdput(in);
 	}

	return error;
}
