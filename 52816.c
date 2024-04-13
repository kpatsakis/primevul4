static int snd_usbmidi_create_endpoints(struct snd_usb_midi *umidi,
					struct snd_usb_midi_endpoint_info *endpoints)
{
	int i, j, err;
	int out_ports = 0, in_ports = 0;

	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i) {
		if (endpoints[i].out_cables) {
			err = snd_usbmidi_out_endpoint_create(umidi,
							      &endpoints[i],
							      &umidi->endpoints[i]);
			if (err < 0)
				return err;
		}
		if (endpoints[i].in_cables) {
			err = snd_usbmidi_in_endpoint_create(umidi,
							     &endpoints[i],
							     &umidi->endpoints[i]);
			if (err < 0)
				return err;
		}

		for (j = 0; j < 0x10; ++j) {
			if (endpoints[i].out_cables & (1 << j)) {
				snd_usbmidi_init_substream(umidi,
							   SNDRV_RAWMIDI_STREAM_OUTPUT,
							   out_ports,
							   &umidi->endpoints[i].out->ports[j].substream);
				++out_ports;
			}
			if (endpoints[i].in_cables & (1 << j)) {
				snd_usbmidi_init_substream(umidi,
							   SNDRV_RAWMIDI_STREAM_INPUT,
							   in_ports,
							   &umidi->endpoints[i].in->ports[j].substream);
				++in_ports;
			}
		}
	}
	dev_dbg(&umidi->dev->dev, "created %d output and %d input ports\n",
		    out_ports, in_ports);
	return 0;
}
