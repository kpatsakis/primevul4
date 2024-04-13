int snd_ctl_create(struct snd_card *card)
{
	static struct snd_device_ops ops = {
		.dev_free = snd_ctl_dev_free,
		.dev_register =	snd_ctl_dev_register,
		.dev_disconnect = snd_ctl_dev_disconnect,
	};

	if (snd_BUG_ON(!card))
		return -ENXIO;
	return snd_device_new(card, SNDRV_DEV_CONTROL, card, &ops);
}
