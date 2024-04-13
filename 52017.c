static int proc_disconnectsignal_compat(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_disconnectsignal32 ds;

	if (copy_from_user(&ds, arg, sizeof(ds)))
		return -EFAULT;
	ps->discsignr = ds.signr;
	ps->disccontext = compat_ptr(ds.context);
	return 0;
}
