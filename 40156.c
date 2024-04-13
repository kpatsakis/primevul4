SYSCALL_DEFINE1(uselib, const char __user *, library)
{
	struct file *file;
	struct filename *tmp = getname(library);
	int error = PTR_ERR(tmp);
	static const struct open_flags uselib_flags = {
		.open_flag = O_LARGEFILE | O_RDONLY | __FMODE_EXEC,
		.acc_mode = MAY_READ | MAY_EXEC | MAY_OPEN,
		.intent = LOOKUP_OPEN,
		.lookup_flags = LOOKUP_FOLLOW,
	};

	if (IS_ERR(tmp))
		goto out;

	file = do_filp_open(AT_FDCWD, tmp, &uselib_flags);
	putname(tmp);
	error = PTR_ERR(file);
	if (IS_ERR(file))
		goto out;

	error = -EINVAL;
	if (!S_ISREG(file_inode(file)->i_mode))
		goto exit;

	error = -EACCES;
	if (file->f_path.mnt->mnt_flags & MNT_NOEXEC)
		goto exit;

	fsnotify_open(file);

	error = -ENOEXEC;
	if(file->f_op) {
		struct linux_binfmt * fmt;

		read_lock(&binfmt_lock);
		list_for_each_entry(fmt, &formats, lh) {
			if (!fmt->load_shlib)
				continue;
			if (!try_module_get(fmt->module))
				continue;
			read_unlock(&binfmt_lock);
			error = fmt->load_shlib(file);
			read_lock(&binfmt_lock);
			put_binfmt(fmt);
			if (error != -ENOEXEC)
				break;
		}
		read_unlock(&binfmt_lock);
	}
exit:
	fput(file);
out:
  	return error;
}
