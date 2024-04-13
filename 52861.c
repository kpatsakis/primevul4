static void snd_usbmidi_rawmidi_free(struct snd_rawmidi *rmidi)
{
	struct snd_usb_midi *umidi = rmidi->private_data;
	snd_usbmidi_free(umidi);
}
