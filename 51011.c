static int ffs_set_inst_name(struct usb_function_instance *fi, const char *name)
{
	struct f_fs_opts *opts;
	char *ptr;
	const char *tmp;
	int name_len, ret;

	name_len = strlen(name) + 1;
	if (name_len > MAX_INST_NAME_LEN)
		return -ENAMETOOLONG;

	ptr = kstrndup(name, name_len, GFP_KERNEL);
	if (!ptr)
		return -ENOMEM;

	opts = to_f_fs_opts(fi);
	tmp = NULL;

	ffs_dev_lock();

	tmp = opts->dev->name_allocated ? opts->dev->name : NULL;
	ret = _ffs_name_dev(opts->dev, ptr);
	if (ret) {
		kfree(ptr);
		ffs_dev_unlock();
		return ret;
	}
	opts->dev->name_allocated = true;

	ffs_dev_unlock();

	kfree(tmp);

	return 0;
}
