int usb_get_status(struct usb_device *dev, int type, int target, void *data)
{
	int ret;
	__le16 *status = kmalloc(sizeof(*status), GFP_KERNEL);

	if (!status)
		return -ENOMEM;

	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		USB_REQ_GET_STATUS, USB_DIR_IN | type, 0, target, status,
		sizeof(*status), USB_CTRL_GET_TIMEOUT);

	if (ret == 2) {
		*(u16 *) data = le16_to_cpu(*status);
		ret = 0;
	} else if (ret >= 0) {
		ret = -EIO;
	}
	kfree(status);
	return ret;
}
