static void show_instructions(struct pt_regs *regs)
{
	int i;
	unsigned long pc = regs->nip - (instructions_to_print * 3 / 4 *
			sizeof(int));

	printk("Instruction dump:");

	for (i = 0; i < instructions_to_print; i++) {
		int instr;

		if (!(i % 8))
			printk("\n");

#if !defined(CONFIG_BOOKE)
		/* If executing with the IMMU off, adjust pc rather
		 * than print XXXXXXXX.
		 */
		if (!(regs->msr & MSR_IR))
			pc = (unsigned long)phys_to_virt(pc);
#endif

		if (!__kernel_text_address(pc) ||
		     probe_kernel_address((unsigned int __user *)pc, instr)) {
			printk(KERN_CONT "XXXXXXXX ");
		} else {
			if (regs->nip == pc)
				printk(KERN_CONT "<%08x> ", instr);
			else
				printk(KERN_CONT "%08x ", instr);
		}

		pc += sizeof(int);
	}

	printk("\n");
}
