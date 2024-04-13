static int snd_usbmidi_input_open(struct snd_rawmidi_substream *substream)
{
	return substream_open(substream, 1, 1);
}
