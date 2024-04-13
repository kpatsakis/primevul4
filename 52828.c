static void snd_usbmidi_error_timer(unsigned long data)
{
	struct snd_usb_midi *umidi = (struct snd_usb_midi *)data;
	unsigned int i, j;

	spin_lock(&umidi->disc_lock);
	if (umidi->disconnected) {
		spin_unlock(&umidi->disc_lock);
		return;
	}
	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i) {
		struct snd_usb_midi_in_endpoint *in = umidi->endpoints[i].in;
		if (in && in->error_resubmit) {
			in->error_resubmit = 0;
			for (j = 0; j < INPUT_URBS; ++j) {
				if (atomic_read(&in->urbs[j]->use_count))
					continue;
				in->urbs[j]->dev = umidi->dev;
				snd_usbmidi_submit_urb(in->urbs[j], GFP_ATOMIC);
			}
		}
		if (umidi->endpoints[i].out)
			snd_usbmidi_do_output(umidi->endpoints[i].out);
	}
	spin_unlock(&umidi->disc_lock);
}
