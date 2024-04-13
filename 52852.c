static void snd_usbmidi_out_tasklet(unsigned long data)
{
	struct snd_usb_midi_out_endpoint *ep =
		(struct snd_usb_midi_out_endpoint *) data;

	snd_usbmidi_do_output(ep);
}
