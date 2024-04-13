int snd_seq_event_port_attach(int client,
			      struct snd_seq_port_callback *pcbp,
			      int cap, int type, int midi_channels,
			      int midi_voices, char *portname)
{
	struct snd_seq_port_info portinfo;
	int  ret;

	/* Set up the port */
	memset(&portinfo, 0, sizeof(portinfo));
	portinfo.addr.client = client;
	strlcpy(portinfo.name, portname ? portname : "Unamed port",
		sizeof(portinfo.name));

	portinfo.capability = cap;
	portinfo.type = type;
	portinfo.kernel = pcbp;
	portinfo.midi_channels = midi_channels;
	portinfo.midi_voices = midi_voices;

	/* Create it */
	ret = snd_seq_kernel_client_ctl(client,
					SNDRV_SEQ_IOCTL_CREATE_PORT,
					&portinfo);

	if (ret >= 0)
		ret = portinfo.addr.port;

	return ret;
}
