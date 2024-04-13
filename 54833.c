static int snd_usb_audio_dev_free(struct snd_device *device)
{
	struct snd_usb_audio *chip = device->device_data;
	return snd_usb_audio_free(chip);
}
