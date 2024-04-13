aiptek_get_report(struct aiptek *aiptek,
		  unsigned char report_type,
		  unsigned char report_id, void *buffer, int size)
{
	return usb_control_msg(aiptek->usbdev,
			       usb_rcvctrlpipe(aiptek->usbdev, 0),
			       USB_REQ_GET_REPORT,
			       USB_TYPE_CLASS | USB_RECIP_INTERFACE |
			       USB_DIR_IN, (report_type << 8) + report_id,
			       aiptek->ifnum, buffer, size, 5000);
}
