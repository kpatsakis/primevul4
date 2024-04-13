show_country(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct hid_device *hdev = to_hid_device(dev);

	return sprintf(buf, "%02x\n", hdev->country & 0xff);
}
