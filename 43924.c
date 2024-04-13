static ssize_t picolcd_operation_mode_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct picolcd_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%hu\n", data->opmode_delay);
}
