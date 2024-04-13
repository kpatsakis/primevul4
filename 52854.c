static int snd_usbmidi_output_close(struct snd_rawmidi_substream *substream)
{
	return substream_open(substream, 0, 0);
}
