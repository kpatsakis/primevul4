static int snd_seq_ioctl_set_queue_tempo(struct snd_seq_client *client,
					 void __user *arg)
{
	int result;
	struct snd_seq_queue_tempo tempo;

	if (copy_from_user(&tempo, arg, sizeof(tempo)))
		return -EFAULT;

	result = snd_seq_set_queue_tempo(client->number, &tempo);
	return result < 0 ? result : 0;
}
