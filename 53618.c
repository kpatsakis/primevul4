void usb_hub_release_all_ports(struct usb_device *hdev, struct usb_dev_state *owner)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(hdev);
	int n;

	for (n = 0; n < hdev->maxchild; n++) {
		if (hub->ports[n]->port_owner == owner)
			hub->ports[n]->port_owner = NULL;
	}

}
