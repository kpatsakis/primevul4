struct usb_device *usb_hub_find_child(struct usb_device *hdev,
		int port1)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(hdev);

	if (port1 < 1 || port1 > hdev->maxchild)
		return NULL;
	return hub->ports[port1 - 1]->child;
}
