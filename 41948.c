static void zap_locks(void)
{
	static unsigned long oops_timestamp;

	if (time_after_eq(jiffies, oops_timestamp) &&
			!time_after(jiffies, oops_timestamp + 30 * HZ))
		return;

	oops_timestamp = jiffies;

	debug_locks_off();
	/* If a crash is occurring, make sure we can't deadlock */
	raw_spin_lock_init(&logbuf_lock);
	/* And make sure that we print immediately */
	sema_init(&console_sem, 1);
}
