static void snd_usbmidi_raw_input(struct snd_usb_midi_in_endpoint *ep,
				  uint8_t *buffer, int buffer_length)
{
	snd_usbmidi_input_data(ep, 0, buffer, buffer_length);
}
