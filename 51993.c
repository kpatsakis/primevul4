static int connected(struct usb_dev_state *ps)
{
	return (!list_empty(&ps->list) &&
			ps->dev->state != USB_STATE_NOTATTACHED);
}
