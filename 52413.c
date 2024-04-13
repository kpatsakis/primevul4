static ssize_t ims_pcu_ofn_reg_data_store(struct device *dev,
					  struct device_attribute *dattr,
					  const char *buf, size_t count)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	int error;
	u8 value;

	error = kstrtou8(buf, 0, &value);
	if (error)
		return error;

	mutex_lock(&pcu->cmd_mutex);
	error = ims_pcu_write_ofn_config(pcu, pcu->ofn_reg_addr, value);
	mutex_unlock(&pcu->cmd_mutex);

	return error ?: count;
}
