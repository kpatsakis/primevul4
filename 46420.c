static inline int current_is_64bit(void)
{
	/*
	 * We can't use test_thread_flag() here because we may be on an
	 * interrupt stack, and the thread flags don't get copied over
	 * from the thread_info on the main stack to the interrupt stack.
	 */
	return !test_ti_thread_flag(task_thread_info(current), TIF_32BIT);
}
