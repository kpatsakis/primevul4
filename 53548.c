static int find_port_owner(struct usb_device *hdev, unsigned port1,
		struct usb_dev_state ***ppowner)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(hdev);

	if (hdev->state == USB_STATE_NOTATTACHED)
		return -ENODEV;
	if (port1 == 0 || port1 > hdev->maxchild)
		return -EINVAL;

	/* Devices not managed by the hub driver
	 * will always have maxchild equal to 0.
	 */
	*ppowner = &(hub->ports[port1 - 1]->port_owner);
	return 0;
}
