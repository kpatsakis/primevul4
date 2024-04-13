static struct snd_rawmidi_substream *snd_usbmidi_find_substream(struct snd_usb_midi *umidi,
								int stream,
								int number)
{
	struct snd_rawmidi_substream *substream;

	list_for_each_entry(substream, &umidi->rmidi->streams[stream].substreams,
			    list) {
		if (substream->number == number)
			return substream;
	}
	return NULL;
}
