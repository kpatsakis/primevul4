struct file *open_exec(const char *name)
{
	struct filename *filename = getname_kernel(name);
	struct file *f = ERR_CAST(filename);

	if (!IS_ERR(filename)) {
		f = do_open_execat(AT_FDCWD, filename, 0);
		putname(filename);
	}
	return f;
}
