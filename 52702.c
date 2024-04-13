int snd_timer_open(struct snd_timer_instance **ti,
		   char *owner, struct snd_timer_id *tid,
		   unsigned int slave_id)
{
	struct snd_timer *timer;
	struct snd_timer_instance *timeri = NULL;

	if (tid->dev_class == SNDRV_TIMER_CLASS_SLAVE) {
		/* open a slave instance */
		if (tid->dev_sclass <= SNDRV_TIMER_SCLASS_NONE ||
		    tid->dev_sclass > SNDRV_TIMER_SCLASS_OSS_SEQUENCER) {
			pr_debug("ALSA: timer: invalid slave class %i\n",
				 tid->dev_sclass);
			return -EINVAL;
		}
		mutex_lock(&register_mutex);
		timeri = snd_timer_instance_new(owner, NULL);
		if (!timeri) {
			mutex_unlock(&register_mutex);
			return -ENOMEM;
		}
		timeri->slave_class = tid->dev_sclass;
		timeri->slave_id = tid->device;
		timeri->flags |= SNDRV_TIMER_IFLG_SLAVE;
		list_add_tail(&timeri->open_list, &snd_timer_slave_list);
		snd_timer_check_slave(timeri);
		mutex_unlock(&register_mutex);
		*ti = timeri;
		return 0;
	}

	/* open a master instance */
	mutex_lock(&register_mutex);
	timer = snd_timer_find(tid);
#ifdef CONFIG_MODULES
	if (!timer) {
		mutex_unlock(&register_mutex);
		snd_timer_request(tid);
		mutex_lock(&register_mutex);
		timer = snd_timer_find(tid);
	}
#endif
	if (!timer) {
		mutex_unlock(&register_mutex);
		return -ENODEV;
	}
	if (!list_empty(&timer->open_list_head)) {
		timeri = list_entry(timer->open_list_head.next,
				    struct snd_timer_instance, open_list);
		if (timeri->flags & SNDRV_TIMER_IFLG_EXCLUSIVE) {
			mutex_unlock(&register_mutex);
			return -EBUSY;
		}
	}
	timeri = snd_timer_instance_new(owner, timer);
	if (!timeri) {
		mutex_unlock(&register_mutex);
		return -ENOMEM;
	}
	timeri->slave_class = tid->dev_sclass;
	timeri->slave_id = slave_id;
	if (list_empty(&timer->open_list_head) && timer->hw.open)
		timer->hw.open(timer);
	list_add_tail(&timeri->open_list, &timer->open_list_head);
	snd_timer_check_master(timeri);
	mutex_unlock(&register_mutex);
	*ti = timeri;
	return 0;
}
