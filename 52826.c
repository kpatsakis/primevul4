static void snd_usbmidi_emagic_input(struct snd_usb_midi_in_endpoint *ep,
				     uint8_t *buffer, int buffer_length)
{
	int i;

	/* FF indicates end of valid data */
	for (i = 0; i < buffer_length; ++i)
		if (buffer[i] == 0xff) {
			buffer_length = i;
			break;
		}

	/* handle F5 at end of last buffer */
	if (ep->seen_f5)
		goto switch_port;

	while (buffer_length > 0) {
		/* determine size of data until next F5 */
		for (i = 0; i < buffer_length; ++i)
			if (buffer[i] == 0xf5)
				break;
		snd_usbmidi_input_data(ep, ep->current_port, buffer, i);
		buffer += i;
		buffer_length -= i;

		if (buffer_length <= 0)
			break;
		/* assert(buffer[0] == 0xf5); */
		ep->seen_f5 = 1;
		++buffer;
		--buffer_length;

	switch_port:
		if (buffer_length <= 0)
			break;
		if (buffer[0] < 0x80) {
			ep->current_port = (buffer[0] - 1) & 15;
			++buffer;
			--buffer_length;
		}
		ep->seen_f5 = 0;
	}
}
