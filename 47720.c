int do_pipe_flags(int *fd, int flags)
{
	struct file *files[2];
	int error = __do_pipe_flags(fd, files, flags);
	if (!error) {
		fd_install(fd[0], files[0]);
		fd_install(fd[1], files[1]);
	}
	return error;
}
