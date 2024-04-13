static void unaccount_freq_event(void)
{
	if (tick_nohz_full_enabled())
		unaccount_freq_event_nohz();
	else
		atomic_dec(&nr_freq_events);
}
