static void snd_usbmidi_in_endpoint_delete(struct snd_usb_midi_in_endpoint *ep)
{
	unsigned int i;

	for (i = 0; i < INPUT_URBS; ++i)
		if (ep->urbs[i])
			free_urb_and_buffer(ep->umidi, ep->urbs[i],
					    ep->urbs[i]->transfer_buffer_length);
	kfree(ep);
}
