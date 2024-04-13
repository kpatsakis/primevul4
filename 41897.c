static int ti_vread_sync(struct usb_device *dev, __u8 request,
				__u16 value, __u16 index, u8 *data, int size)
{
	int status;

	status = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0), request,
			(USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_IN),
			value, index, data, size, 1000);
	if (status < 0)
		return status;
	if (status != size) {
		dev_dbg(&dev->dev, "%s - wanted to write %d, but only wrote %d\n",
			__func__, size, status);
		return -ECOMM;
	}
	return 0;
}
