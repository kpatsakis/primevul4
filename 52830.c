static void snd_usbmidi_free(struct snd_usb_midi *umidi)
{
	int i;

	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i) {
		struct snd_usb_midi_endpoint *ep = &umidi->endpoints[i];
		if (ep->out)
			snd_usbmidi_out_endpoint_delete(ep->out);
		if (ep->in)
			snd_usbmidi_in_endpoint_delete(ep->in);
	}
	mutex_destroy(&umidi->mutex);
	kfree(umidi);
}
