static ssize_t show_tabletXtilt(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);

	if (aiptek->curSetting.xTilt == AIPTEK_TILT_DISABLE) {
		return snprintf(buf, PAGE_SIZE, "disable\n");
	} else {
		return snprintf(buf, PAGE_SIZE, "%d\n",
				aiptek->curSetting.xTilt);
	}
}
