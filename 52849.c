static void snd_usbmidi_out_endpoint_clear(struct snd_usb_midi_out_endpoint *ep)
{
	unsigned int i;

	for (i = 0; i < OUTPUT_URBS; ++i)
		if (ep->urbs[i].urb) {
			free_urb_and_buffer(ep->umidi, ep->urbs[i].urb,
					    ep->max_transfer);
			ep->urbs[i].urb = NULL;
		}
}
