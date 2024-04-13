static void snd_usbmidi_init_substream(struct snd_usb_midi *umidi,
				       int stream, int number,
				       struct snd_rawmidi_substream **rsubstream)
{
	struct port_info *port_info;
	const char *name_format;

	struct snd_rawmidi_substream *substream =
		snd_usbmidi_find_substream(umidi, stream, number);
	if (!substream) {
		dev_err(&umidi->dev->dev, "substream %d:%d not found\n", stream,
			number);
		return;
	}

	/* TODO: read port name from jack descriptor */
	port_info = find_port_info(umidi, number);
	name_format = port_info ? port_info->name : "%s MIDI %d";
	snprintf(substream->name, sizeof(substream->name),
		 name_format, umidi->card->shortname, number + 1);

	*rsubstream = substream;
}
