static void account_freq_event_nohz(void)
{
#ifdef CONFIG_NO_HZ_FULL
	/* Lock so we don't race with concurrent unaccount */
	spin_lock(&nr_freq_lock);
	if (atomic_inc_return(&nr_freq_events) == 1)
		tick_nohz_dep_set(TICK_DEP_BIT_PERF_EVENTS);
	spin_unlock(&nr_freq_lock);
#endif
}
