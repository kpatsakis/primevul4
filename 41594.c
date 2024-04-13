static int wdm_manage_power(struct usb_interface *intf, int on)
{
	/* need autopm_get/put here to ensure the usbcore sees the new value */
	int rv = usb_autopm_get_interface(intf);
	if (rv < 0)
		goto err;

	intf->needs_remote_wakeup = on;
	usb_autopm_put_interface(intf);
err:
	return rv;
}
