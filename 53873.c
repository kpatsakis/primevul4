store_tabletPointerMode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);
	int new_mode = map_str_to_val(pointer_mode_map, buf, count);

	if (new_mode == AIPTEK_INVALID_VALUE)
		return -EINVAL;

	aiptek->newSetting.pointerMode = new_mode;
	return count;
}
