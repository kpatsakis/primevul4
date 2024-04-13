static void snd_timer_proc_read(struct snd_info_entry *entry,
				struct snd_info_buffer *buffer)
{
	struct snd_timer *timer;
	struct snd_timer_instance *ti;

	mutex_lock(&register_mutex);
	list_for_each_entry(timer, &snd_timer_list, device_list) {
		switch (timer->tmr_class) {
		case SNDRV_TIMER_CLASS_GLOBAL:
			snd_iprintf(buffer, "G%i: ", timer->tmr_device);
			break;
		case SNDRV_TIMER_CLASS_CARD:
			snd_iprintf(buffer, "C%i-%i: ",
				    timer->card->number, timer->tmr_device);
			break;
		case SNDRV_TIMER_CLASS_PCM:
			snd_iprintf(buffer, "P%i-%i-%i: ", timer->card->number,
				    timer->tmr_device, timer->tmr_subdevice);
			break;
		default:
			snd_iprintf(buffer, "?%i-%i-%i-%i: ", timer->tmr_class,
				    timer->card ? timer->card->number : -1,
				    timer->tmr_device, timer->tmr_subdevice);
		}
		snd_iprintf(buffer, "%s :", timer->name);
		if (timer->hw.resolution)
			snd_iprintf(buffer, " %lu.%03luus (%lu ticks)",
				    timer->hw.resolution / 1000,
				    timer->hw.resolution % 1000,
				    timer->hw.ticks);
		if (timer->hw.flags & SNDRV_TIMER_HW_SLAVE)
			snd_iprintf(buffer, " SLAVE");
		snd_iprintf(buffer, "\n");
		list_for_each_entry(ti, &timer->open_list_head, open_list)
			snd_iprintf(buffer, "  Client %s : %s\n",
				    ti->owner ? ti->owner : "unknown",
				    ti->flags & (SNDRV_TIMER_IFLG_START |
						 SNDRV_TIMER_IFLG_RUNNING)
				    ? "running" : "stopped");
	}
	mutex_unlock(&register_mutex);
}
