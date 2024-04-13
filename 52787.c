int snd_seq_kernel_client_ctl(int clientid, unsigned int cmd, void *arg)
{
	struct snd_seq_client *client;
	mm_segment_t fs;
	int result;

	client = clientptr(clientid);
	if (client == NULL)
		return -ENXIO;
	fs = snd_enter_user();
	result = snd_seq_do_ioctl(client, cmd, (void __force __user *)arg);
	snd_leave_user(fs);
	return result;
}
