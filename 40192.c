struct file *open_exec(const char *name)
{
	struct file *file;
	int err;
	struct filename tmp = { .name = name };
	static const struct open_flags open_exec_flags = {
		.open_flag = O_LARGEFILE | O_RDONLY | __FMODE_EXEC,
		.acc_mode = MAY_EXEC | MAY_OPEN,
		.intent = LOOKUP_OPEN,
		.lookup_flags = LOOKUP_FOLLOW,
	};

	file = do_filp_open(AT_FDCWD, &tmp, &open_exec_flags);
	if (IS_ERR(file))
		goto out;

	err = -EACCES;
	if (!S_ISREG(file_inode(file)->i_mode))
		goto exit;

	if (file->f_path.mnt->mnt_flags & MNT_NOEXEC)
		goto exit;

	fsnotify_open(file);

	err = deny_write_access(file);
	if (err)
		goto exit;

out:
	return file;

exit:
	fput(file);
	return ERR_PTR(err);
}
