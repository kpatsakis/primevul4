store_tabletStylusLower(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);
	int new_button = map_str_to_val(stylus_button_map, buf, count);

	if (new_button == AIPTEK_INVALID_VALUE)
		return -EINVAL;

	aiptek->newSetting.stylusButtonLower = new_button;
	return count;
}
