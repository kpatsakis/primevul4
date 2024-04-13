static ssize_t show_tabletSize(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%dx%d\n",
			input_abs_get_max(aiptek->inputdev, ABS_X) + 1,
			input_abs_get_max(aiptek->inputdev, ABS_Y) + 1);
}
