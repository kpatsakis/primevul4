static u64 perf_ustack_task_size(struct pt_regs *regs)
{
	unsigned long addr = perf_user_stack_pointer(regs);

	if (!addr || addr >= TASK_SIZE)
		return 0;

	return TASK_SIZE - addr;
}
