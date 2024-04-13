int snd_seq_queue_timer_set_tempo(int queueid, int client,
				  struct snd_seq_queue_tempo *info)
{
	struct snd_seq_queue *q = queueptr(queueid);
	int result;

	if (q == NULL)
		return -EINVAL;
	if (! queue_access_lock(q, client)) {
		queuefree(q);
		return -EPERM;
	}

	result = snd_seq_timer_set_tempo(q->timer, info->tempo);
	if (result >= 0)
		result = snd_seq_timer_set_ppq(q->timer, info->ppq);
	if (result >= 0 && info->skew_base > 0)
		result = snd_seq_timer_set_skew(q->timer, info->skew_value,
						info->skew_base);
	queue_access_unlock(q);
	queuefree(q);
	return result;
}
