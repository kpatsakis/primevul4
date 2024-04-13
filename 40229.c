static int ptrace_trapping_sleep_fn(void *flags)
{
	schedule();
	return 0;
}
