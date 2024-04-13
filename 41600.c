static int wdm_resume(struct usb_interface *intf)
{
	struct wdm_device *desc = wdm_find_device(intf);
	int rv;

	dev_dbg(&desc->intf->dev, "wdm%d_resume\n", intf->minor);

	clear_bit(WDM_SUSPENDING, &desc->flags);
	rv = recover_from_urb_loss(desc);

	return rv;
}
