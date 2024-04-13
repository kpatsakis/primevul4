static ssize_t ap_depth_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	return snprintf(buf, PAGE_SIZE, "%d\n", ap_dev->queue_depth);
}
