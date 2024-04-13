int snd_usb_autoresume(struct snd_usb_audio *chip)
{
	if (atomic_read(&chip->shutdown))
		return -EIO;
	if (atomic_inc_return(&chip->active) == 1)
		return usb_autopm_get_interface(chip->pm_intf);
	return 0;
}
