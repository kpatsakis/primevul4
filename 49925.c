static ssize_t ap_modalias_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "ap:t%02X", to_ap_dev(dev)->device_type);
}
