static int snd_compress_dev_register(struct snd_device *device)
{
	int ret = -EINVAL;
	char str[16];
	struct snd_compr *compr;

	if (snd_BUG_ON(!device || !device->device_data))
		return -EBADFD;
	compr = device->device_data;

	sprintf(str, "comprC%iD%i", compr->card->number, compr->device);
	pr_debug("reg %s for device %s, direction %d\n", str, compr->name,
			compr->direction);
	/* register compressed device */
	ret = snd_register_device(SNDRV_DEVICE_TYPE_COMPRESS, compr->card,
			compr->device, &snd_compr_file_ops, compr, str);
	if (ret < 0) {
		pr_err("snd_register_device failed\n %d", ret);
		return ret;
	}
	return ret;

}
