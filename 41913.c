void __sched console_conditional_schedule(void)
{
	if (console_may_schedule)
		cond_resched();
}
