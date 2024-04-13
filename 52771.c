static int snd_seq_ioctl_get_queue_status(struct snd_seq_client *client,
					  void __user *arg)
{
	struct snd_seq_queue_status status;
	struct snd_seq_queue *queue;
	struct snd_seq_timer *tmr;

	if (copy_from_user(&status, arg, sizeof(status)))
		return -EFAULT;

	queue = queueptr(status.queue);
	if (queue == NULL)
		return -EINVAL;
	memset(&status, 0, sizeof(status));
	status.queue = queue->queue;
	
	tmr = queue->timer;
	status.events = queue->tickq->cells + queue->timeq->cells;

	status.time = snd_seq_timer_get_cur_time(tmr);
	status.tick = snd_seq_timer_get_cur_tick(tmr);

	status.running = tmr->running;

	status.flags = queue->flags;
	queuefree(queue);

	if (copy_to_user(arg, &status, sizeof(status)))
		return -EFAULT;
	return 0;
}
