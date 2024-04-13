static int proc_disconnectsignal(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_disconnectsignal ds;

	if (copy_from_user(&ds, arg, sizeof(ds)))
		return -EFAULT;
	ps->discsignr = ds.signr;
	ps->disccontext = ds.context;
	return 0;
}
