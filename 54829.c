static int __usb_audio_resume(struct usb_interface *intf, bool reset_resume)
{
	struct snd_usb_audio *chip = usb_get_intfdata(intf);
	struct usb_mixer_interface *mixer;
	struct list_head *p;
	int err = 0;

	if (chip == (void *)-1L)
		return 0;
	if (--chip->num_suspended_intf)
		return 0;

	atomic_inc(&chip->active); /* avoid autopm */
	/*
	 * ALSA leaves material resumption to user space
	 * we just notify and restart the mixers
	 */
	list_for_each_entry(mixer, &chip->mixer_list, list) {
		err = snd_usb_mixer_resume(mixer, reset_resume);
		if (err < 0)
			goto err_out;
	}

	list_for_each(p, &chip->midi_list) {
		snd_usbmidi_resume(p);
	}

	if (!chip->autosuspended)
		snd_power_change_state(chip->card, SNDRV_CTL_POWER_D0);
	chip->autosuspended = 0;

err_out:
	atomic_dec(&chip->active); /* allow autopm after this point */
	return err;
}
