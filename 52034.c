static int proc_submiturb(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_urb uurb;

	if (copy_from_user(&uurb, arg, sizeof(uurb)))
		return -EFAULT;

	return proc_do_submiturb(ps, &uurb,
			(((struct usbdevfs_urb __user *)arg)->iso_frame_desc),
			arg);
}
