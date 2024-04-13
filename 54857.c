static int snd_seq_device_dev_register(struct snd_device *device)
{
	struct snd_seq_device *dev = device->device_data;
	int err;

	err = device_add(&dev->dev);
	if (err < 0)
		return err;
	if (!dev->dev.driver)
		queue_autoload_drivers();
	return 0;
}
