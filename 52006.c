static int parse_usbdevfs_streams(struct usb_dev_state *ps,
				  struct usbdevfs_streams __user *streams,
				  unsigned int *num_streams_ret,
				  unsigned int *num_eps_ret,
				  struct usb_host_endpoint ***eps_ret,
				  struct usb_interface **intf_ret)
{
	unsigned int i, num_streams, num_eps;
	struct usb_host_endpoint **eps;
	struct usb_interface *intf = NULL;
	unsigned char ep;
	int ifnum, ret;

	if (get_user(num_streams, &streams->num_streams) ||
	    get_user(num_eps, &streams->num_eps))
		return -EFAULT;

	if (num_eps < 1 || num_eps > USB_MAXENDPOINTS)
		return -EINVAL;

	/* The XHCI controller allows max 2 ^ 16 streams */
	if (num_streams_ret && (num_streams < 2 || num_streams > 65536))
		return -EINVAL;

	eps = kmalloc(num_eps * sizeof(*eps), GFP_KERNEL);
	if (!eps)
		return -ENOMEM;

	for (i = 0; i < num_eps; i++) {
		if (get_user(ep, &streams->eps[i])) {
			ret = -EFAULT;
			goto error;
		}
		eps[i] = ep_to_host_endpoint(ps->dev, ep);
		if (!eps[i]) {
			ret = -EINVAL;
			goto error;
		}

		/* usb_alloc/free_streams operate on an usb_interface */
		ifnum = findintfep(ps->dev, ep);
		if (ifnum < 0) {
			ret = ifnum;
			goto error;
		}

		if (i == 0) {
			ret = checkintf(ps, ifnum);
			if (ret < 0)
				goto error;
			intf = usb_ifnum_to_if(ps->dev, ifnum);
		} else {
			/* Verify all eps belong to the same interface */
			if (ifnum != intf->altsetting->desc.bInterfaceNumber) {
				ret = -EINVAL;
				goto error;
			}
		}
	}

	if (num_streams_ret)
		*num_streams_ret = num_streams;
	*num_eps_ret = num_eps;
	*eps_ret = eps;
	*intf_ret = intf;

	return 0;

error:
	kfree(eps);
	return ret;
}
