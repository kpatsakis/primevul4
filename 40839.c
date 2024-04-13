static void print_fatal_signal(int signr)
{
	struct pt_regs *regs = signal_pt_regs();
	printk(KERN_INFO "%s/%d: potentially unexpected fatal signal %d.\n",
		current->comm, task_pid_nr(current), signr);

#if defined(__i386__) && !defined(__arch_um__)
	printk(KERN_INFO "code at %08lx: ", regs->ip);
	{
		int i;
		for (i = 0; i < 16; i++) {
			unsigned char insn;

			if (get_user(insn, (unsigned char *)(regs->ip + i)))
				break;
			printk(KERN_CONT "%02x ", insn);
		}
	}
	printk(KERN_CONT "\n");
#endif
	preempt_disable();
	show_regs(regs);
	preempt_enable();
}
