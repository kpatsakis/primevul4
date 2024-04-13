void do_send_trap(struct pt_regs *regs, unsigned long address,
		  unsigned long error_code, int signal_code, int breakpt)
{
	siginfo_t info;

	current->thread.trap_nr = signal_code;
	if (notify_die(DIE_DABR_MATCH, "dabr_match", regs, error_code,
			11, SIGSEGV) == NOTIFY_STOP)
		return;

	/* Deliver the signal to userspace */
	info.si_signo = SIGTRAP;
	info.si_errno = breakpt;	/* breakpoint or watchpoint id */
	info.si_code = signal_code;
	info.si_addr = (void __user *)address;
	force_sig_info(SIGTRAP, &info, current);
}
