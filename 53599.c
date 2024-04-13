bool usb_device_is_owned(struct usb_device *udev)
{
	struct usb_hub *hub;

	if (udev->state == USB_STATE_NOTATTACHED || !udev->parent)
		return false;
	hub = usb_hub_to_struct_hub(udev->parent);
	return !!hub->ports[udev->portnum - 1]->port_owner;
}
