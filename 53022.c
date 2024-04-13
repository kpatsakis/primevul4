static int ati_remote2_post_reset(struct usb_interface *interface)
{
	struct ati_remote2 *ar2;
	struct usb_host_interface *alt = interface->cur_altsetting;
	int r = 0;

	if (alt->desc.bInterfaceNumber)
		return 0;

	ar2 = usb_get_intfdata(interface);

	dev_dbg(&ar2->intf[0]->dev, "%s()\n", __func__);

	if (ar2->flags == ATI_REMOTE2_OPENED)
		r = ati_remote2_submit_urbs(ar2);

	mutex_unlock(&ati_remote2_mutex);

	return r;
}
