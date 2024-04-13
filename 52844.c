static void snd_usbmidi_input_trigger(struct snd_rawmidi_substream *substream,
				      int up)
{
	struct snd_usb_midi *umidi = substream->rmidi->private_data;

	if (up)
		set_bit(substream->number, &umidi->input_triggered);
	else
		clear_bit(substream->number, &umidi->input_triggered);
}
