static int dev_match_devt(struct device *dev, void *data)
{
	dev_t *devt = data;
	return dev->devt == *devt;
}
