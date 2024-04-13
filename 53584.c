static int hub_usb3_port_disable(struct usb_hub *hub, int port1)
{
	int ret;
	int total_time;
	u16 portchange, portstatus;

	if (!hub_is_superspeed(hub->hdev))
		return -EINVAL;

	ret = hub_port_status(hub, port1, &portstatus, &portchange);
	if (ret < 0)
		return ret;

	/*
	 * USB controller Advanced Micro Devices, Inc. [AMD] FCH USB XHCI
	 * Controller [1022:7814] will have spurious result making the following
	 * usb 3.0 device hotplugging route to the 2.0 root hub and recognized
	 * as high-speed device if we set the usb 3.0 port link state to
	 * Disabled. Since it's already in USB_SS_PORT_LS_RX_DETECT state, we
	 * check the state here to avoid the bug.
	 */
	if ((portstatus & USB_PORT_STAT_LINK_STATE) ==
				USB_SS_PORT_LS_RX_DETECT) {
		dev_dbg(&hub->ports[port1 - 1]->dev,
			 "Not disabling port; link state is RxDetect\n");
		return ret;
	}

	ret = hub_set_port_link_state(hub, port1, USB_SS_PORT_LS_SS_DISABLED);
	if (ret)
		return ret;

	/* Wait for the link to enter the disabled state. */
	for (total_time = 0; ; total_time += HUB_DEBOUNCE_STEP) {
		ret = hub_port_status(hub, port1, &portstatus, &portchange);
		if (ret < 0)
			return ret;

		if ((portstatus & USB_PORT_STAT_LINK_STATE) ==
				USB_SS_PORT_LS_SS_DISABLED)
			break;
		if (total_time >= HUB_DEBOUNCE_TIMEOUT)
			break;
		msleep(HUB_DEBOUNCE_STEP);
	}
	if (total_time >= HUB_DEBOUNCE_TIMEOUT)
		dev_warn(&hub->ports[port1 - 1]->dev,
				"Could not disable after %d ms\n", total_time);

	return hub_set_port_link_state(hub, port1, USB_SS_PORT_LS_RX_DETECT);
}
