hub_clear_tt_buffer(struct usb_device *hdev, u16 devinfo, u16 tt)
{
	/* Need to clear both directions for control ep */
	if (((devinfo >> 11) & USB_ENDPOINT_XFERTYPE_MASK) ==
			USB_ENDPOINT_XFER_CONTROL) {
		int status = usb_control_msg(hdev, usb_sndctrlpipe(hdev, 0),
				HUB_CLEAR_TT_BUFFER, USB_RT_PORT,
				devinfo ^ 0x8000, tt, NULL, 0, 1000);
		if (status)
			return status;
	}
	return usb_control_msg(hdev, usb_sndctrlpipe(hdev, 0),
			       HUB_CLEAR_TT_BUFFER, USB_RT_PORT, devinfo,
			       tt, NULL, 0, 1000);
}
