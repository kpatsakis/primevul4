static int snd_seq_bus_match(struct device *dev, struct device_driver *drv)
{
	struct snd_seq_device *sdev = to_seq_dev(dev);
	struct snd_seq_driver *sdrv = to_seq_drv(drv);

	return strcmp(sdrv->id, sdev->id) == 0 &&
		sdrv->argsize == sdev->argsize;
}
