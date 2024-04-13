static int proc_control_compat(struct usb_dev_state *ps,
				struct usbdevfs_ctrltransfer32 __user *p32)
{
	struct usbdevfs_ctrltransfer __user *p;
	__u32 udata;
	p = compat_alloc_user_space(sizeof(*p));
	if (copy_in_user(p, p32, (sizeof(*p32) - sizeof(compat_caddr_t))) ||
	    get_user(udata, &p32->data) ||
	    put_user(compat_ptr(udata), &p->data))
		return -EFAULT;
	return proc_control(ps, p);
}
