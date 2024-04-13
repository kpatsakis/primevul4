static ssize_t show_tabletCoordinateMode(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n",
			map_val_to_str(coordinate_mode_map,
					aiptek->curSetting.coordinateMode));
}
