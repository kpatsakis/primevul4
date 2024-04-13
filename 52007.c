static int proc_alloc_streams(struct usb_dev_state *ps, void __user *arg)
{
	unsigned num_streams, num_eps;
	struct usb_host_endpoint **eps;
	struct usb_interface *intf;
	int r;

	r = parse_usbdevfs_streams(ps, arg, &num_streams, &num_eps,
				   &eps, &intf);
	if (r)
		return r;

	destroy_async_on_interface(ps,
				   intf->altsetting[0].desc.bInterfaceNumber);

	r = usb_alloc_streams(intf, eps, num_eps, num_streams, GFP_KERNEL);
	kfree(eps);
	return r;
}
