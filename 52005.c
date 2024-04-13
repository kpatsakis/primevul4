static int match_devt(struct device *dev, void *data)
{
	return dev->devt == (dev_t) (unsigned long) data;
}
