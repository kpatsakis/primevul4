static void hub_set_initial_usb2_lpm_policy(struct usb_device *udev)
{
	struct usb_hub *hub = usb_hub_to_struct_hub(udev->parent);
	int connect_type = USB_PORT_CONNECT_TYPE_UNKNOWN;

	if (!udev->usb2_hw_lpm_capable)
		return;

	if (hub)
		connect_type = hub->ports[udev->portnum - 1]->connect_type;

	if ((udev->bos->ext_cap->bmAttributes & cpu_to_le32(USB_BESL_SUPPORT)) ||
			connect_type == USB_PORT_CONNECT_TYPE_HARD_WIRED) {
		udev->usb2_hw_lpm_allowed = 1;
		usb_set_usb2_hardware_lpm(udev, 1);
	}
}
