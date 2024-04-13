static int proc_ioctl_default(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_ioctl	ctrl;

	if (copy_from_user(&ctrl, arg, sizeof(ctrl)))
		return -EFAULT;
	return proc_ioctl(ps, &ctrl);
}
