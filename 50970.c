static inline struct f_fs_opts *ffs_do_functionfs_bind(struct usb_function *f,
						struct usb_configuration *c)
{
	struct ffs_function *func = ffs_func_from_usb(f);
	struct f_fs_opts *ffs_opts =
		container_of(f->fi, struct f_fs_opts, func_inst);
	int ret;

	ENTER();

	/*
	 * Legacy gadget triggers binding in functionfs_ready_callback,
	 * which already uses locking; taking the same lock here would
	 * cause a deadlock.
	 *
	 * Configfs-enabled gadgets however do need ffs_dev_lock.
	 */
	if (!ffs_opts->no_configfs)
		ffs_dev_lock();
	ret = ffs_opts->dev->desc_ready ? 0 : -ENODEV;
	func->ffs = ffs_opts->dev->ffs_data;
	if (!ffs_opts->no_configfs)
		ffs_dev_unlock();
	if (ret)
		return ERR_PTR(ret);

	func->conf = c;
	func->gadget = c->cdev->gadget;

	/*
	 * in drivers/usb/gadget/configfs.c:configfs_composite_bind()
	 * configurations are bound in sequence with list_for_each_entry,
	 * in each configuration its functions are bound in sequence
	 * with list_for_each_entry, so we assume no race condition
	 * with regard to ffs_opts->bound access
	 */
	if (!ffs_opts->refcnt) {
		ret = functionfs_bind(func->ffs, c->cdev);
		if (ret)
			return ERR_PTR(ret);
	}
	ffs_opts->refcnt++;
	func->function.strings = func->ffs->stringtabs;

	return ffs_opts;
}
