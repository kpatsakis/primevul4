static int proc_free_streams(struct usb_dev_state *ps, void __user *arg)
{
	unsigned num_eps;
	struct usb_host_endpoint **eps;
	struct usb_interface *intf;
	int r;

	r = parse_usbdevfs_streams(ps, arg, NULL, &num_eps, &eps, &intf);
	if (r)
		return r;

	destroy_async_on_interface(ps,
				   intf->altsetting[0].desc.bInterfaceNumber);

	r = usb_free_streams(intf, eps, num_eps, GFP_KERNEL);
	kfree(eps);
	return r;
}
