static int snd_usbmidi_create_rawmidi(struct snd_usb_midi *umidi,
				      int out_ports, int in_ports)
{
	struct snd_rawmidi *rmidi;
	int err;

	err = snd_rawmidi_new(umidi->card, "USB MIDI",
			      umidi->next_midi_device++,
			      out_ports, in_ports, &rmidi);
	if (err < 0)
		return err;
	strcpy(rmidi->name, umidi->card->shortname);
	rmidi->info_flags = SNDRV_RAWMIDI_INFO_OUTPUT |
			    SNDRV_RAWMIDI_INFO_INPUT |
			    SNDRV_RAWMIDI_INFO_DUPLEX;
	rmidi->ops = &snd_usbmidi_ops;
	rmidi->private_data = umidi;
	rmidi->private_free = snd_usbmidi_rawmidi_free;
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_OUTPUT,
			    &snd_usbmidi_output_ops);
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_INPUT,
			    &snd_usbmidi_input_ops);

	umidi->rmidi = rmidi;
	return 0;
}
