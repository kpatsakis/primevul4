static int wdm_flush(struct file *file, fl_owner_t id)
{
	struct wdm_device *desc = file->private_data;

	wait_event(desc->wait, !test_bit(WDM_IN_USE, &desc->flags));

	/* cannot dereference desc->intf if WDM_DISCONNECTING */
	if (desc->werr < 0 && !test_bit(WDM_DISCONNECTING, &desc->flags))
		dev_err(&desc->intf->dev, "Error in flush path: %d\n",
			desc->werr);

	return usb_translate_errors(desc->werr);
}
