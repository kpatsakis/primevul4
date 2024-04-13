static void unaccount_freq_event_nohz(void)
{
#ifdef CONFIG_NO_HZ_FULL
	spin_lock(&nr_freq_lock);
	if (atomic_dec_and_test(&nr_freq_events))
		tick_nohz_dep_clear(TICK_DEP_BIT_PERF_EVENTS);
	spin_unlock(&nr_freq_lock);
#endif
}
