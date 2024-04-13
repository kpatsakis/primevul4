static int send_cmd(struct usb_device *dev, __u8 command,
				__u8 moduleid, __u16 value, u8 *data,
				int size)
{
	return ti_vsend_sync(dev, command, value, moduleid, data, size,
			TI_VSEND_TIMEOUT_DEFAULT);
}
