ims_pcu_update_firmware_status_show(struct device *dev,
				    struct device_attribute *dattr,
				    char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);

	return scnprintf(buf, PAGE_SIZE, "%d\n", pcu->update_firmware_status);
}
