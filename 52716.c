static struct snd_seq_queue *queue_new(int owner, int locked)
{
	struct snd_seq_queue *q;

	q = kzalloc(sizeof(*q), GFP_KERNEL);
	if (!q)
		return NULL;

	spin_lock_init(&q->owner_lock);
	spin_lock_init(&q->check_lock);
	mutex_init(&q->timer_mutex);
	snd_use_lock_init(&q->use_lock);
	q->queue = -1;

	q->tickq = snd_seq_prioq_new();
	q->timeq = snd_seq_prioq_new();
	q->timer = snd_seq_timer_new();
	if (q->tickq == NULL || q->timeq == NULL || q->timer == NULL) {
		snd_seq_prioq_delete(&q->tickq);
		snd_seq_prioq_delete(&q->timeq);
		snd_seq_timer_delete(&q->timer);
		kfree(q);
		return NULL;
	}

	q->owner = owner;
	q->locked = locked;
	q->klocked = 0;

	return q;
}
