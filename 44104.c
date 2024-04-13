int can_do_mlock(void)
{
	if (capable(CAP_IPC_LOCK))
		return 1;
	if (rlimit(RLIMIT_MEMLOCK) != 0)
		return 1;
	return 0;
}
