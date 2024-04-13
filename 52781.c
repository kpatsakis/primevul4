static int snd_seq_ioctl_set_queue_client(struct snd_seq_client *client,
					  void __user *arg)
{
	int err;
	struct snd_seq_queue_client info;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	if (info.used >= 0) {
		err = snd_seq_queue_use(info.queue, client->number, info.used);
		if (err < 0)
			return err;
	}

	return snd_seq_ioctl_get_queue_client(client, arg);
}
