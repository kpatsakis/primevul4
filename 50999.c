static int ffs_func_set_alt(struct usb_function *f,
			    unsigned interface, unsigned alt)
{
	struct ffs_function *func = ffs_func_from_usb(f);
	struct ffs_data *ffs = func->ffs;
	int ret = 0, intf;

	if (alt != (unsigned)-1) {
		intf = ffs_func_revmap_intf(func, interface);
		if (unlikely(intf < 0))
			return intf;
	}

	if (ffs->func)
		ffs_func_eps_disable(ffs->func);

	if (ffs->state == FFS_DEACTIVATED) {
		ffs->state = FFS_CLOSING;
		INIT_WORK(&ffs->reset_work, ffs_reset_work);
		schedule_work(&ffs->reset_work);
		return -ENODEV;
	}

	if (ffs->state != FFS_ACTIVE)
		return -ENODEV;

	if (alt == (unsigned)-1) {
		ffs->func = NULL;
		ffs_event_add(ffs, FUNCTIONFS_DISABLE);
		return 0;
	}

	ffs->func = func;
	ret = ffs_func_eps_enable(func);
	if (likely(ret >= 0))
		ffs_event_add(ffs, FUNCTIONFS_ENABLE);
	return ret;
}
