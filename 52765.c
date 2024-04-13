static int snd_seq_ioctl_delete_queue(struct snd_seq_client *client,
				      void __user *arg)
{
	struct snd_seq_queue_info info;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	return snd_seq_queue_delete(client->number, info.queue);
}
