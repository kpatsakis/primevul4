static int recover_from_urb_loss(struct wdm_device *desc)
{
	int rv = 0;

	if (desc->count) {
		rv = usb_submit_urb(desc->validity, GFP_NOIO);
		if (rv < 0)
			dev_err(&desc->intf->dev,
				"Error resume submitting int urb - %d\n", rv);
	}
	return rv;
}
