void snd_usb_autosuspend(struct snd_usb_audio *chip)
{
	if (atomic_read(&chip->shutdown))
		return;
	if (atomic_dec_and_test(&chip->active))
		usb_autopm_put_interface(chip->pm_intf);
}
