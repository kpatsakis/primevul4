static void snd_usbmidi_us122l_input(struct snd_usb_midi_in_endpoint *ep,
				     uint8_t *buffer, int buffer_length)
{
	if (buffer_length != 9)
		return;
	buffer_length = 8;
	while (buffer_length && buffer[buffer_length - 1] == 0xFD)
		buffer_length--;
	if (buffer_length)
		snd_usbmidi_input_data(ep, 0, buffer, buffer_length);
}
