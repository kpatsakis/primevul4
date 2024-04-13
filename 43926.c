static ssize_t picolcd_operation_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct picolcd_data *data = dev_get_drvdata(dev);

	if (data->status & PICOLCD_BOOTLOADER)
		return snprintf(buf, PAGE_SIZE, "[bootloader] lcd\n");
	else
		return snprintf(buf, PAGE_SIZE, "bootloader [lcd]\n");
}
