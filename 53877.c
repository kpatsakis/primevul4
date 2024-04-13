store_tabletWheel(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);
	int err, w;

	err = kstrtoint(buf, 10, &w);
	if (err)
		return err;

	aiptek->newSetting.wheel = w;
	return count;
}
