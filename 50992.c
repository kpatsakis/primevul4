static int ffs_func_bind(struct usb_configuration *c,
			 struct usb_function *f)
{
	struct f_fs_opts *ffs_opts = ffs_do_functionfs_bind(f, c);
	struct ffs_function *func = ffs_func_from_usb(f);
	int ret;

	if (IS_ERR(ffs_opts))
		return PTR_ERR(ffs_opts);

	ret = _ffs_func_bind(c, f);
	if (ret && !--ffs_opts->refcnt)
		functionfs_unbind(func->ffs);

	return ret;
}
