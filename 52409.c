static ssize_t ims_pcu_ofn_bit_show(struct device *dev,
				    struct device_attribute *dattr,
				    char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct ims_pcu_ofn_bit_attribute *attr =
		container_of(dattr, struct ims_pcu_ofn_bit_attribute, dattr);
	int error;
	u8 data;

	mutex_lock(&pcu->cmd_mutex);
	error = ims_pcu_read_ofn_config(pcu, attr->addr, &data);
	mutex_unlock(&pcu->cmd_mutex);

	if (error)
		return error;

	return scnprintf(buf, PAGE_SIZE, "%d\n", !!(data & (1 << attr->nr)));
}
