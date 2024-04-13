static void kvm_send_hwpoison_signal(unsigned long address, struct task_struct *tsk)
{
	siginfo_t info;

	info.si_signo	= SIGBUS;
	info.si_errno	= 0;
	info.si_code	= BUS_MCEERR_AR;
	info.si_addr	= (void __user *)address;
	info.si_addr_lsb = PAGE_SHIFT;

	send_sig_info(SIGBUS, &info, tsk);
}
