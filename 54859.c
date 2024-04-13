int snd_seq_device_new(struct snd_card *card, int device, const char *id,
		       int argsize, struct snd_seq_device **result)
{
	struct snd_seq_device *dev;
	int err;
	static struct snd_device_ops dops = {
		.dev_free = snd_seq_device_dev_free,
		.dev_register = snd_seq_device_dev_register,
		.dev_disconnect = snd_seq_device_dev_disconnect,
	};

	if (result)
		*result = NULL;

	if (snd_BUG_ON(!id))
		return -EINVAL;

	dev = kzalloc(sizeof(*dev) + argsize, GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	/* set up device info */
	dev->card = card;
	dev->device = device;
	dev->id = id;
	dev->argsize = argsize;

	device_initialize(&dev->dev);
	dev->dev.parent = &card->card_dev;
	dev->dev.bus = &snd_seq_bus_type;
	dev->dev.release = snd_seq_dev_release;
	dev_set_name(&dev->dev, "%s-%d-%d", dev->id, card->number, device);

	/* add this device to the list */
	err = snd_device_new(card, SNDRV_DEV_SEQUENCER, dev, &dops);
	if (err < 0) {
		put_device(&dev->dev);
		return err;
	}
	
	if (result)
		*result = dev;

	return 0;
}
