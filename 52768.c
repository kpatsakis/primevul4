static int snd_seq_ioctl_get_named_queue(struct snd_seq_client *client, void __user *arg)
{
	struct snd_seq_queue_info info;
	struct snd_seq_queue *q;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	q = snd_seq_queue_find_name(info.name);
	if (q == NULL)
		return -EINVAL;
	info.queue = q->queue;
	info.owner = q->owner;
	info.locked = q->locked;
	queuefree(q);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}
