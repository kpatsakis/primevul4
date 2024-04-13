static int ti_vsend_sync(struct usb_device *dev, u8 request, u16 value,
		u16 index, u8 *data, int size, int timeout)
{
	int status;

	status = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), request,
			(USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_DIR_OUT),
			value, index, data, size, timeout);
	if (status < 0)
		return status;
	if (status != size) {
		dev_dbg(&dev->dev, "%s - wanted to write %d, but only wrote %d\n",
			__func__, size, status);
		return -ECOMM;
	}
	return 0;
}
