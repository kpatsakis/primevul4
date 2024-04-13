static void snd_usbmidi_ftdi_input(struct snd_usb_midi_in_endpoint *ep,
				   uint8_t *buffer, int buffer_length)
{
	if (buffer_length > 2)
		snd_usbmidi_input_data(ep, 0, buffer + 2, buffer_length - 2);
}
