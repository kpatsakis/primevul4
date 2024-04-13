static int snd_compress_dev_disconnect(struct snd_device *device)
{
	struct snd_compr *compr;

	compr = device->device_data;
	snd_unregister_device(SNDRV_DEVICE_TYPE_COMPRESS, compr->card,
		compr->device);
	return 0;
}
