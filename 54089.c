static int snd_compress_remove_device(struct snd_compr *device)
{
	return snd_card_free(device->card);
}
