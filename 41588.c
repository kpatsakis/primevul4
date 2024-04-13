struct usb_driver *usb_cdc_wdm_register(struct usb_interface *intf,
					struct usb_endpoint_descriptor *ep,
					int bufsize,
					int (*manage_power)(struct usb_interface *, int))
{
	int rv = -EINVAL;

	rv = wdm_create(intf, ep, bufsize, manage_power);
	if (rv < 0)
		goto err;

	return &wdm_driver;
err:
	return ERR_PTR(rv);
}
