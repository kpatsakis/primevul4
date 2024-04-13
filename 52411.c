static ssize_t ims_pcu_ofn_reg_addr_show(struct device *dev,
					 struct device_attribute *dattr,
					 char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	int error;

	mutex_lock(&pcu->cmd_mutex);
	error = scnprintf(buf, PAGE_SIZE, "%x\n", pcu->ofn_reg_addr);
	mutex_unlock(&pcu->cmd_mutex);

	return error;
}
