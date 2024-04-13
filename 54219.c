asmlinkage void __exception do_sp_pc_abort(unsigned long addr,
					   unsigned int esr,
					   struct pt_regs *regs)
{
	struct siginfo info;

	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code  = BUS_ADRALN;
	info.si_addr  = (void __user *)addr;
	arm64_notify_die("", regs, &info, esr);
}
