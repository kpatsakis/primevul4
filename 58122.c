static int gs_cmd_reset(struct gs_usb *gsusb, struct gs_can *gsdev)
{
	struct gs_device_mode *dm;
	struct usb_interface *intf = gsdev->iface;
	int rc;

	dm = kzalloc(sizeof(*dm), GFP_KERNEL);
	if (!dm)
		return -ENOMEM;

	dm->mode = GS_CAN_MODE_RESET;

	rc = usb_control_msg(interface_to_usbdev(intf),
			     usb_sndctrlpipe(interface_to_usbdev(intf), 0),
			     GS_USB_BREQ_MODE,
			     USB_DIR_OUT|USB_TYPE_VENDOR|USB_RECIP_INTERFACE,
			     gsdev->channel,
			     0,
			     dm,
			     sizeof(*dm),
			     1000);

	return rc;
}
