static ssize_t ims_pcu_ofn_bit_store(struct device *dev,
				     struct device_attribute *dattr,
				     const char *buf, size_t count)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct ims_pcu_ofn_bit_attribute *attr =
		container_of(dattr, struct ims_pcu_ofn_bit_attribute, dattr);
	int error;
	int value;
	u8 data;

	error = kstrtoint(buf, 0, &value);
	if (error)
		return error;

	if (value > 1)
		return -EINVAL;

	mutex_lock(&pcu->cmd_mutex);

	error = ims_pcu_read_ofn_config(pcu, attr->addr, &data);
	if (!error) {
		if (value)
			data |= 1U << attr->nr;
		else
			data &= ~(1U << attr->nr);

		error = ims_pcu_write_ofn_config(pcu, attr->addr, data);
	}

	mutex_unlock(&pcu->cmd_mutex);

	return error ?: count;
}
