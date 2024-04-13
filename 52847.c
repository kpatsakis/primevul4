static void snd_usbmidi_novation_input(struct snd_usb_midi_in_endpoint *ep,
				       uint8_t *buffer, int buffer_length)
{
	if (buffer_length < 2 || !buffer[0] || buffer_length < buffer[0] + 1)
		return;
	snd_usbmidi_input_data(ep, 0, &buffer[2], buffer[0] - 1);
}
