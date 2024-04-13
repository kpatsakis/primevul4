static umode_t ims_pcu_is_attr_visible(struct kobject *kobj,
				       struct attribute *attr, int n)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	umode_t mode = attr->mode;

	if (pcu->bootloader_mode) {
		if (attr != &dev_attr_update_firmware_status.attr &&
		    attr != &dev_attr_update_firmware.attr &&
		    attr != &dev_attr_reset_device.attr) {
			mode = 0;
		}
	} else {
		if (attr == &dev_attr_update_firmware_status.attr)
			mode = 0;
	}

	return mode;
}
