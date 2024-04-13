static int hub_set_port_link_state(struct usb_hub *hub, int port1,
			unsigned int link_status)
{
	return set_port_feature(hub->hdev,
			port1 | (link_status << 3),
			USB_PORT_FEAT_LINK_STATE);
}
