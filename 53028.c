static ssize_t ati_remote2_show_channel_mask(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	struct usb_device *udev = to_usb_device(dev);
	struct usb_interface *intf = usb_ifnum_to_if(udev, 0);
	struct ati_remote2 *ar2 = usb_get_intfdata(intf);

	return sprintf(buf, "0x%04x\n", ar2->channel_mask);
}
