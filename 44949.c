struct file *filp_open(const char *filename, int flags, int mode)
{
	return do_filp_open(AT_FDCWD, filename, flags, mode, 0);
}
