static int snd_timer_start1(struct snd_timer *timer, struct snd_timer_instance *timeri,
			    unsigned long sticks)
{
	list_move_tail(&timeri->active_list, &timer->active_list_head);
	if (timer->running) {
		if (timer->hw.flags & SNDRV_TIMER_HW_SLAVE)
			goto __start_now;
		timer->flags |= SNDRV_TIMER_FLG_RESCHED;
		timeri->flags |= SNDRV_TIMER_IFLG_START;
		return 1;	/* delayed start */
	} else {
		timer->sticks = sticks;
		timer->hw.start(timer);
	      __start_now:
		timer->running++;
		timeri->flags |= SNDRV_TIMER_IFLG_RUNNING;
		return 0;
	}
}
