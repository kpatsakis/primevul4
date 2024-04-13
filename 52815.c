static void snd_usbmidi_cme_input(struct snd_usb_midi_in_endpoint *ep,
				  uint8_t *buffer, int buffer_length)
{
	if (buffer_length < 2 || (buffer[0] & 0x0f) != 0x0f)
		snd_usbmidi_standard_input(ep, buffer, buffer_length);
	else
		snd_usbmidi_input_data(ep, buffer[0] >> 4,
				       &buffer[1], buffer_length - 1);
}
