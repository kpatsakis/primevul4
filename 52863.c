static void snd_usbmidi_standard_input(struct snd_usb_midi_in_endpoint *ep,
				       uint8_t *buffer, int buffer_length)
{
	int i;

	for (i = 0; i + 3 < buffer_length; i += 4)
		if (buffer[i] != 0) {
			int cable = buffer[i] >> 4;
			int length = snd_usbmidi_cin_length[buffer[i] & 0x0f];
			snd_usbmidi_input_data(ep, cable, &buffer[i + 1],
					       length);
		}
}
