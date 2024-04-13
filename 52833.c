static void snd_usbmidi_get_port_info(struct snd_rawmidi *rmidi, int number,
				      struct snd_seq_port_info *seq_port_info)
{
	struct snd_usb_midi *umidi = rmidi->private_data;
	struct port_info *port_info;

	/* TODO: read port flags from descriptors */
	port_info = find_port_info(umidi, number);
	if (port_info) {
		seq_port_info->type = port_info->seq_flags;
		seq_port_info->midi_voices = port_info->voices;
	}
}
