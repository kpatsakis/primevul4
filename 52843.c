void snd_usbmidi_input_stop(struct list_head *p)
{
	struct snd_usb_midi *umidi;
	unsigned int i, j;

	umidi = list_entry(p, struct snd_usb_midi, list);
	if (!umidi->input_running)
		return;
	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i) {
		struct snd_usb_midi_endpoint *ep = &umidi->endpoints[i];
		if (ep->in)
			for (j = 0; j < INPUT_URBS; ++j)
				usb_kill_urb(ep->in->urbs[j]);
	}
	umidi->input_running = 0;
}
