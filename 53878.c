store_tabletXtilt(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);
	int x;

	if (kstrtoint(buf, 10, &x)) {
		size_t len = buf[count - 1] == '\n' ? count - 1 : count;

		if (strncmp(buf, "disable", len))
			return -EINVAL;

		aiptek->newSetting.xTilt = AIPTEK_TILT_DISABLE;
	} else {
		if (x < AIPTEK_TILT_MIN || x > AIPTEK_TILT_MAX)
			return -EINVAL;

		aiptek->newSetting.xTilt = x;
	}

	return count;
}
