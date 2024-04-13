void snd_usbmidi_input_start(struct list_head *p)
{
	struct snd_usb_midi *umidi;
	int i;

	umidi = list_entry(p, struct snd_usb_midi, list);
	if (umidi->input_running || !umidi->opened[1])
		return;
	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i)
		snd_usbmidi_input_start_ep(umidi->endpoints[i].in);
	umidi->input_running = 1;
}
