static int snd_seq_ioctl_get_queue_client(struct snd_seq_client *client,
					  void __user *arg)
{
	struct snd_seq_queue_client info;
	int used;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	used = snd_seq_queue_is_used(info.queue, client->number);
	if (used < 0)
		return -EINVAL;
	info.used = used;
	info.client = client->number;

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}
