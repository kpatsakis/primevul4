void show_regs(struct pt_regs * regs)
{
	int i, trap;

	show_regs_print_info(KERN_DEFAULT);

	printk("NIP: "REG" LR: "REG" CTR: "REG"\n",
	       regs->nip, regs->link, regs->ctr);
	printk("REGS: %p TRAP: %04lx   %s  (%s)\n",
	       regs, regs->trap, print_tainted(), init_utsname()->release);
	printk("MSR: "REG" ", regs->msr);
	printbits(regs->msr, msr_bits);
	printk("  CR: %08lx  XER: %08lx\n", regs->ccr, regs->xer);
	trap = TRAP(regs);
	if ((regs->trap != 0xc00) && cpu_has_feature(CPU_FTR_CFAR))
		printk("CFAR: "REG" ", regs->orig_gpr3);
	if (trap == 0x200 || trap == 0x300 || trap == 0x600)
#if defined(CONFIG_4xx) || defined(CONFIG_BOOKE)
		printk("DEAR: "REG" ESR: "REG" ", regs->dar, regs->dsisr);
#else
		printk("DAR: "REG" DSISR: %08lx ", regs->dar, regs->dsisr);
#endif
#ifdef CONFIG_PPC64
	printk("SOFTE: %ld ", regs->softe);
#endif
#ifdef CONFIG_PPC_TRANSACTIONAL_MEM
	if (MSR_TM_ACTIVE(regs->msr))
		printk("\nPACATMSCRATCH: %016llx ", get_paca()->tm_scratch);
#endif

	for (i = 0;  i < 32;  i++) {
		if ((i % REGS_PER_LINE) == 0)
			printk("\nGPR%02d: ", i);
		printk(REG " ", regs->gpr[i]);
		if (i == LAST_VOLATILE && !FULL_REGS(regs))
			break;
	}
	printk("\n");
#ifdef CONFIG_KALLSYMS
	/*
	 * Lookup NIP late so we have the best change of getting the
	 * above info out without failing
	 */
	printk("NIP ["REG"] %pS\n", regs->nip, (void *)regs->nip);
	printk("LR ["REG"] %pS\n", regs->link, (void *)regs->link);
#endif
	show_stack(current, (unsigned long *) regs->gpr[1]);
	if (!user_mode(regs))
		show_instructions(regs);
}
