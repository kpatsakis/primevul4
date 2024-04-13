static ssize_t show_tabletEventsReceived(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct aiptek *aiptek = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%ld\n", aiptek->eventCount);
}
