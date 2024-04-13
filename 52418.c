static ssize_t ims_pcu_reset_device(struct device *dev,
				    struct device_attribute *dattr,
				    const char *buf, size_t count)
{
	static const u8 reset_byte = 1;
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	int value;
	int error;

	error = kstrtoint(buf, 0, &value);
	if (error)
		return error;

	if (value != 1)
		return -EINVAL;

	dev_info(pcu->dev, "Attempting to reset device\n");

	error = ims_pcu_execute_command(pcu, PCU_RESET, &reset_byte, 1);
	if (error) {
		dev_info(pcu->dev,
			 "Failed to reset device, error: %d\n",
			 error);
		return error;
	}

	return count;
}
