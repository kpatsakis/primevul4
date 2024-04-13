static void do_jobctl_trap(void)
{
	struct signal_struct *signal = current->signal;
	int signr = current->jobctl & JOBCTL_STOP_SIGMASK;

	if (current->ptrace & PT_SEIZED) {
		if (!signal->group_stop_count &&
		    !(signal->flags & SIGNAL_STOP_STOPPED))
			signr = SIGTRAP;
		WARN_ON_ONCE(!signr);
		ptrace_do_notify(signr, signr | (PTRACE_EVENT_STOP << 8),
				 CLD_STOPPED);
	} else {
		WARN_ON_ONCE(!signr);
		ptrace_stop(signr, CLD_STOPPED, 0, NULL);
		current->exit_code = 0;
	}
}
