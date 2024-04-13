static int proc_setintf(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_setinterface setintf;
	int ret;

	if (copy_from_user(&setintf, arg, sizeof(setintf)))
		return -EFAULT;
	ret = checkintf(ps, setintf.interface);
	if (ret)
		return ret;

	destroy_async_on_interface(ps, setintf.interface);

	return usb_set_interface(ps->dev, setintf.interface,
			setintf.altsetting);
}
