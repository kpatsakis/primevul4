int snd_seq_get_port_info(struct snd_seq_client_port * port,
			  struct snd_seq_port_info * info)
{
	if (snd_BUG_ON(!port || !info))
		return -EINVAL;

	/* get port name */
	strlcpy(info->name, port->name, sizeof(info->name));
	
	/* get capabilities */
	info->capability = port->capability;

	/* get port type */
	info->type = port->type;

	/* information about supported channels/voices */
	info->midi_channels = port->midi_channels;
	info->midi_voices = port->midi_voices;
	info->synth_voices = port->synth_voices;

	/* get subscriber counts */
	info->read_use = port->c_src.count;
	info->write_use = port->c_dest.count;
	
	/* timestamping */
	info->flags = 0;
	if (port->timestamping) {
		info->flags |= SNDRV_SEQ_PORT_FLG_TIMESTAMP;
		if (port->time_real)
			info->flags |= SNDRV_SEQ_PORT_FLG_TIME_REAL;
		info->time_queue = port->time_queue;
	}

	return 0;
}
