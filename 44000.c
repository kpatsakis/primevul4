check_options()
{
	if (logfile_fd >= 0 && logfile_fd != log_to_fd)
		close(logfile_fd);
}
