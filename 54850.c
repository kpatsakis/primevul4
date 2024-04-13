static int request_seq_drv(struct device *dev, void *data)
{
	struct snd_seq_device *sdev = to_seq_dev(dev);

	if (!dev->driver)
		request_module("snd-%s", sdev->id);
	return 0;
}
