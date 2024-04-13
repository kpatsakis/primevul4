static void snd_timer_tasklet(unsigned long arg)
{
	struct snd_timer *timer = (struct snd_timer *) arg;
	struct snd_timer_instance *ti;
	struct list_head *p;
	unsigned long resolution, ticks;
	unsigned long flags;

	spin_lock_irqsave(&timer->lock, flags);
	/* now process all callbacks */
	while (!list_empty(&timer->sack_list_head)) {
		p = timer->sack_list_head.next;		/* get first item */
		ti = list_entry(p, struct snd_timer_instance, ack_list);

		/* remove from ack_list and make empty */
		list_del_init(p);

		ticks = ti->pticks;
		ti->pticks = 0;
		resolution = ti->resolution;

		ti->flags |= SNDRV_TIMER_IFLG_CALLBACK;
		spin_unlock(&timer->lock);
		if (ti->callback)
			ti->callback(ti, resolution, ticks);
		spin_lock(&timer->lock);
		ti->flags &= ~SNDRV_TIMER_IFLG_CALLBACK;
	}
	spin_unlock_irqrestore(&timer->lock, flags);
}
