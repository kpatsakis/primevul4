void ptrace_notify(int exit_code)
{
	BUG_ON((exit_code & (0x7f | ~0xffff)) != SIGTRAP);
	if (unlikely(current->task_works))
		task_work_run();

	spin_lock_irq(&current->sighand->siglock);
	ptrace_do_notify(SIGTRAP, exit_code, CLD_TRAPPED);
	spin_unlock_irq(&current->sighand->siglock);
}
