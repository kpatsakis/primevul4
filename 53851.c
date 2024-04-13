static ssize_t show_firmwareCode(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%04x\n",
			aiptek->features.firmwareCode);
}
