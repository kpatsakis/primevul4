static void __do_user_fault(struct task_struct *tsk, unsigned long addr,
			    unsigned int esr, unsigned int sig, int code,
			    struct pt_regs *regs)
{
	struct siginfo si;

	if (show_unhandled_signals && unhandled_signal(tsk, sig) &&
	    printk_ratelimit()) {
		pr_info("%s[%d]: unhandled %s (%d) at 0x%08lx, esr 0x%03x\n",
			tsk->comm, task_pid_nr(tsk), fault_name(esr), sig,
			addr, esr);
		show_pte(tsk->mm, addr);
		show_regs(regs);
	}

	tsk->thread.fault_address = addr;
	tsk->thread.fault_code = esr;
	si.si_signo = sig;
	si.si_errno = 0;
	si.si_code = code;
	si.si_addr = (void __user *)addr;
	force_sig_info(sig, &si, tsk);
}
