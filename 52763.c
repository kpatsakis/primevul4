static int snd_seq_ioctl_create_queue(struct snd_seq_client *client,
				      void __user *arg)
{
	struct snd_seq_queue_info info;
	int result;
	struct snd_seq_queue *q;

	if (copy_from_user(&info, arg, sizeof(info)))
		return -EFAULT;

	result = snd_seq_queue_alloc(client->number, info.locked, info.flags);
	if (result < 0)
		return result;

	q = queueptr(result);
	if (q == NULL)
		return -EINVAL;

	info.queue = q->queue;
	info.locked = q->locked;
	info.owner = q->owner;

	/* set queue name */
	if (! info.name[0])
		snprintf(info.name, sizeof(info.name), "Queue-%d", q->queue);
	strlcpy(q->name, info.name, sizeof(q->name));
	queuefree(q);

	if (copy_to_user(arg, &info, sizeof(info)))
		return -EFAULT;

	return 0;
}
