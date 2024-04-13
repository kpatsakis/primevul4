void snd_usbmidi_resume(struct list_head *p)
{
	struct snd_usb_midi *umidi;

	umidi = list_entry(p, struct snd_usb_midi, list);
	mutex_lock(&umidi->mutex);
	snd_usbmidi_input_start(p);
	mutex_unlock(&umidi->mutex);
}
