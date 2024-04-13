int usb_hub_set_port_power(struct usb_device *hdev, struct usb_hub *hub,
			   int port1, bool set)
{
	int ret;

	if (set)
		ret = set_port_feature(hdev, port1, USB_PORT_FEAT_POWER);
	else
		ret = usb_clear_port_feature(hdev, port1, USB_PORT_FEAT_POWER);

	if (ret)
		return ret;

	if (set)
		set_bit(port1, hub->power_bits);
	else
		clear_bit(port1, hub->power_bits);
	return 0;
}
