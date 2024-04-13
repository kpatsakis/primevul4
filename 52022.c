static int proc_ioctl_compat(struct usb_dev_state *ps, compat_uptr_t arg)
{
	struct usbdevfs_ioctl32 __user *uioc;
	struct usbdevfs_ioctl ctrl;
	u32 udata;

	uioc = compat_ptr((long)arg);
	if (!access_ok(VERIFY_READ, uioc, sizeof(*uioc)) ||
	    __get_user(ctrl.ifno, &uioc->ifno) ||
	    __get_user(ctrl.ioctl_code, &uioc->ioctl_code) ||
	    __get_user(udata, &uioc->data))
		return -EFAULT;
	ctrl.data = compat_ptr(udata);

	return proc_ioctl(ps, &ctrl);
}
