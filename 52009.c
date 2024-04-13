static int proc_bulk_compat(struct usb_dev_state *ps,
			struct usbdevfs_bulktransfer32 __user *p32)
{
	struct usbdevfs_bulktransfer __user *p;
	compat_uint_t n;
	compat_caddr_t addr;

	p = compat_alloc_user_space(sizeof(*p));

	if (get_user(n, &p32->ep) || put_user(n, &p->ep) ||
	    get_user(n, &p32->len) || put_user(n, &p->len) ||
	    get_user(n, &p32->timeout) || put_user(n, &p->timeout) ||
	    get_user(addr, &p32->data) || put_user(compat_ptr(addr), &p->data))
		return -EFAULT;

	return proc_bulk(ps, p);
}
