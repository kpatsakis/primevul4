static int hub_port_disable(struct usb_hub *hub, int port1, int set_state)
{
	struct usb_port *port_dev = hub->ports[port1 - 1];
	struct usb_device *hdev = hub->hdev;
	int ret = 0;

	if (port_dev->child && set_state)
		usb_set_device_state(port_dev->child, USB_STATE_NOTATTACHED);
	if (!hub->error) {
		if (hub_is_superspeed(hub->hdev))
			ret = hub_usb3_port_disable(hub, port1);
		else
			ret = usb_clear_port_feature(hdev, port1,
					USB_PORT_FEAT_ENABLE);
	}
	if (ret && ret != -ENODEV)
		dev_err(&port_dev->dev, "cannot disable (err = %d)\n", ret);
	return ret;
}
