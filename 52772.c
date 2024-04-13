static int snd_seq_ioctl_get_queue_tempo(struct snd_seq_client *client,
					 void __user *arg)
{
	struct snd_seq_queue_tempo tempo;
	struct snd_seq_queue *queue;
	struct snd_seq_timer *tmr;

	if (copy_from_user(&tempo, arg, sizeof(tempo)))
		return -EFAULT;

	queue = queueptr(tempo.queue);
	if (queue == NULL)
		return -EINVAL;
	memset(&tempo, 0, sizeof(tempo));
	tempo.queue = queue->queue;
	
	tmr = queue->timer;

	tempo.tempo = tmr->tempo;
	tempo.ppq = tmr->ppq;
	tempo.skew_value = tmr->skew;
	tempo.skew_base = tmr->skew_base;
	queuefree(queue);

	if (copy_to_user(arg, &tempo, sizeof(tempo)))
		return -EFAULT;
	return 0;
}
