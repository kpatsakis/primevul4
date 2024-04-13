int snd_seq_event_port_detach(int client, int port)
{
	struct snd_seq_port_info portinfo;
	int  err;

	memset(&portinfo, 0, sizeof(portinfo));
	portinfo.addr.client = client;
	portinfo.addr.port   = port;
	err = snd_seq_kernel_client_ctl(client,
					SNDRV_SEQ_IOCTL_DELETE_PORT,
					&portinfo);

	return err;
}
