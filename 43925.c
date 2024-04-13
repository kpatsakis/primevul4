static ssize_t picolcd_operation_mode_delay_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct picolcd_data *data = dev_get_drvdata(dev);
	unsigned u;
	if (sscanf(buf, "%u", &u) != 1)
		return -EINVAL;
	if (u > 30000)
		return -EINVAL;
	else
		data->opmode_delay = u;
	return count;
}
