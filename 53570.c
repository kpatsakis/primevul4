static bool hub_port_warm_reset_required(struct usb_hub *hub, int port1,
		u16 portstatus)
{
	u16 link_state;

	if (!hub_is_superspeed(hub->hdev))
		return false;

	if (test_bit(port1, hub->warm_reset_bits))
		return true;

	link_state = portstatus & USB_PORT_STAT_LINK_STATE;
	return link_state == USB_SS_PORT_LS_SS_INACTIVE
		|| link_state == USB_SS_PORT_LS_COMP_MOD;
}
