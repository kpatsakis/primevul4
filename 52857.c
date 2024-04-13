static int snd_usbmidi_output_open(struct snd_rawmidi_substream *substream)
{
	struct snd_usb_midi *umidi = substream->rmidi->private_data;
	struct usbmidi_out_port *port = NULL;
	int i, j;

	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i)
		if (umidi->endpoints[i].out)
			for (j = 0; j < 0x10; ++j)
				if (umidi->endpoints[i].out->ports[j].substream == substream) {
					port = &umidi->endpoints[i].out->ports[j];
					break;
				}
	if (!port) {
		snd_BUG();
		return -ENXIO;
	}

	substream->runtime->private_data = port;
	port->state = STATE_UNKNOWN;
	return substream_open(substream, 0, 1);
}
