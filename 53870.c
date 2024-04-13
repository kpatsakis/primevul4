store_tabletJitterDelay(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);
	int err, j;

	err = kstrtoint(buf, 10, &j);
	if (err)
		return err;

	aiptek->newSetting.jitterDelay = j;
	return count;
}
