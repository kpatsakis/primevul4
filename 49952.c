static inline void ap_schedule_poll_timer(void)
{
	if (ap_using_interrupts())
		return;
	__ap_schedule_poll_timer();
}
