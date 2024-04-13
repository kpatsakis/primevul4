static void ffs_free_inst(struct usb_function_instance *f)
{
	struct f_fs_opts *opts;

	opts = to_f_fs_opts(f);
	ffs_dev_lock();
	_ffs_free_dev(opts->dev);
	ffs_dev_unlock();
	kfree(opts);
}
