static ssize_t usbip_debug_store(struct device *dev,
				 struct device_attribute *attr, const char *buf,
				 size_t count)
{
	if (sscanf(buf, "%lx", &usbip_debug_flag) != 1)
		return -EINVAL;
	return count;
}
