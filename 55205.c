int snd_seq_set_port_info(struct snd_seq_client_port * port,
			  struct snd_seq_port_info * info)
{
	if (snd_BUG_ON(!port || !info))
		return -EINVAL;

	/* set port name */
	if (info->name[0])
		strlcpy(port->name, info->name, sizeof(port->name));
	
	/* set capabilities */
	port->capability = info->capability;
	
	/* get port type */
	port->type = info->type;

	/* information about supported channels/voices */
	port->midi_channels = info->midi_channels;
	port->midi_voices = info->midi_voices;
	port->synth_voices = info->synth_voices;

	/* timestamping */
	port->timestamping = (info->flags & SNDRV_SEQ_PORT_FLG_TIMESTAMP) ? 1 : 0;
	port->time_real = (info->flags & SNDRV_SEQ_PORT_FLG_TIME_REAL) ? 1 : 0;
	port->time_queue = info->time_queue;

	return 0;
}
