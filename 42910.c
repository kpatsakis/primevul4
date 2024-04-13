void __show_regs(struct pt_regs *regs, int all)
{
	unsigned long cr0 = 0L, cr2 = 0L, cr3 = 0L, cr4 = 0L, fs, gs, shadowgs;
	unsigned long d0, d1, d2, d3, d6, d7;
	unsigned int fsindex, gsindex;
	unsigned int ds, cs, es;

	printk(KERN_DEFAULT "RIP: %04lx:[<%016lx>] ", regs->cs & 0xffff, regs->ip);
	printk_address(regs->ip);
	printk(KERN_DEFAULT "RSP: %04lx:%016lx  EFLAGS: %08lx\n", regs->ss,
			regs->sp, regs->flags);
	printk(KERN_DEFAULT "RAX: %016lx RBX: %016lx RCX: %016lx\n",
	       regs->ax, regs->bx, regs->cx);
	printk(KERN_DEFAULT "RDX: %016lx RSI: %016lx RDI: %016lx\n",
	       regs->dx, regs->si, regs->di);
	printk(KERN_DEFAULT "RBP: %016lx R08: %016lx R09: %016lx\n",
	       regs->bp, regs->r8, regs->r9);
	printk(KERN_DEFAULT "R10: %016lx R11: %016lx R12: %016lx\n",
	       regs->r10, regs->r11, regs->r12);
	printk(KERN_DEFAULT "R13: %016lx R14: %016lx R15: %016lx\n",
	       regs->r13, regs->r14, regs->r15);

	asm("movl %%ds,%0" : "=r" (ds));
	asm("movl %%cs,%0" : "=r" (cs));
	asm("movl %%es,%0" : "=r" (es));
	asm("movl %%fs,%0" : "=r" (fsindex));
	asm("movl %%gs,%0" : "=r" (gsindex));

	rdmsrl(MSR_FS_BASE, fs);
	rdmsrl(MSR_GS_BASE, gs);
	rdmsrl(MSR_KERNEL_GS_BASE, shadowgs);

	if (!all)
		return;

	cr0 = read_cr0();
	cr2 = read_cr2();
	cr3 = read_cr3();
	cr4 = read_cr4();

	printk(KERN_DEFAULT "FS:  %016lx(%04x) GS:%016lx(%04x) knlGS:%016lx\n",
	       fs, fsindex, gs, gsindex, shadowgs);
	printk(KERN_DEFAULT "CS:  %04x DS: %04x ES: %04x CR0: %016lx\n", cs, ds,
			es, cr0);
	printk(KERN_DEFAULT "CR2: %016lx CR3: %016lx CR4: %016lx\n", cr2, cr3,
			cr4);

	get_debugreg(d0, 0);
	get_debugreg(d1, 1);
	get_debugreg(d2, 2);
	get_debugreg(d3, 3);
	get_debugreg(d6, 6);
	get_debugreg(d7, 7);

	/* Only print out debug registers if they are in their non-default state. */
	if ((d0 == 0) && (d1 == 0) && (d2 == 0) && (d3 == 0) &&
	    (d6 == DR6_RESERVED) && (d7 == 0x400))
		return;

	printk(KERN_DEFAULT "DR0: %016lx DR1: %016lx DR2: %016lx\n", d0, d1, d2);
	printk(KERN_DEFAULT "DR3: %016lx DR6: %016lx DR7: %016lx\n", d3, d6, d7);

}
