static void snd_usbmidi_midiman_input(struct snd_usb_midi_in_endpoint *ep,
				      uint8_t *buffer, int buffer_length)
{
	int i;

	for (i = 0; i + 3 < buffer_length; i += 4)
		if (buffer[i + 3] != 0) {
			int port = buffer[i + 3] >> 4;
			int length = buffer[i + 3] & 3;
			snd_usbmidi_input_data(ep, port, &buffer[i], length);
		}
}
