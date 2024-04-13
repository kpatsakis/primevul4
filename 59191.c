static void account_freq_event(void)
{
	if (tick_nohz_full_enabled())
		account_freq_event_nohz();
	else
		atomic_inc(&nr_freq_events);
}
