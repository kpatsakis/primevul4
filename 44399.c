int copy_thread(unsigned long clone_flags, unsigned long usp,
		unsigned long arg, struct task_struct *p)
{
	struct pt_regs *childregs, *kregs;
	extern void ret_from_fork(void);
	extern void ret_from_kernel_thread(void);
	void (*f)(void);
	unsigned long sp = (unsigned long)task_stack_page(p) + THREAD_SIZE;

	/* Copy registers */
	sp -= sizeof(struct pt_regs);
	childregs = (struct pt_regs *) sp;
	if (unlikely(p->flags & PF_KTHREAD)) {
		struct thread_info *ti = (void *)task_stack_page(p);
		memset(childregs, 0, sizeof(struct pt_regs));
		childregs->gpr[1] = sp + sizeof(struct pt_regs);
		childregs->gpr[14] = usp;	/* function */
#ifdef CONFIG_PPC64
		clear_tsk_thread_flag(p, TIF_32BIT);
		childregs->softe = 1;
#endif
		childregs->gpr[15] = arg;
		p->thread.regs = NULL;	/* no user register state */
		ti->flags |= _TIF_RESTOREALL;
		f = ret_from_kernel_thread;
	} else {
		struct pt_regs *regs = current_pt_regs();
		CHECK_FULL_REGS(regs);
		*childregs = *regs;
		if (usp)
			childregs->gpr[1] = usp;
		p->thread.regs = childregs;
		childregs->gpr[3] = 0;  /* Result from fork() */
		if (clone_flags & CLONE_SETTLS) {
#ifdef CONFIG_PPC64
			if (!is_32bit_task())
				childregs->gpr[13] = childregs->gpr[6];
			else
#endif
				childregs->gpr[2] = childregs->gpr[6];
		}

		f = ret_from_fork;
	}
	sp -= STACK_FRAME_OVERHEAD;

	/*
	 * The way this works is that at some point in the future
	 * some task will call _switch to switch to the new task.
	 * That will pop off the stack frame created below and start
	 * the new task running at ret_from_fork.  The new task will
	 * do some house keeping and then return from the fork or clone
	 * system call, using the stack frame created above.
	 */
	((unsigned long *)sp)[0] = 0;
	sp -= sizeof(struct pt_regs);
	kregs = (struct pt_regs *) sp;
	sp -= STACK_FRAME_OVERHEAD;
	p->thread.ksp = sp;
#ifdef CONFIG_PPC32
	p->thread.ksp_limit = (unsigned long)task_stack_page(p) +
				_ALIGN_UP(sizeof(struct thread_info), 16);
#endif
#ifdef CONFIG_HAVE_HW_BREAKPOINT
	p->thread.ptrace_bps[0] = NULL;
#endif

	p->thread.fp_save_area = NULL;
#ifdef CONFIG_ALTIVEC
	p->thread.vr_save_area = NULL;
#endif

#ifdef CONFIG_PPC_STD_MMU_64
	if (mmu_has_feature(MMU_FTR_SLB)) {
		unsigned long sp_vsid;
		unsigned long llp = mmu_psize_defs[mmu_linear_psize].sllp;

		if (mmu_has_feature(MMU_FTR_1T_SEGMENT))
			sp_vsid = get_kernel_vsid(sp, MMU_SEGSIZE_1T)
				<< SLB_VSID_SHIFT_1T;
		else
			sp_vsid = get_kernel_vsid(sp, MMU_SEGSIZE_256M)
				<< SLB_VSID_SHIFT;
		sp_vsid |= SLB_VSID_KERNEL | llp;
		p->thread.ksp_vsid = sp_vsid;
	}
#endif /* CONFIG_PPC_STD_MMU_64 */
#ifdef CONFIG_PPC64 
	if (cpu_has_feature(CPU_FTR_DSCR)) {
		p->thread.dscr_inherit = current->thread.dscr_inherit;
		p->thread.dscr = current->thread.dscr;
	}
	if (cpu_has_feature(CPU_FTR_HAS_PPR))
		p->thread.ppr = INIT_PPR;
#endif
	/*
	 * The PPC64 ABI makes use of a TOC to contain function 
	 * pointers.  The function (ret_from_except) is actually a pointer
	 * to the TOC entry.  The first entry is a pointer to the actual
	 * function.
	 */
#ifdef CONFIG_PPC64
	kregs->nip = *((unsigned long *)f);
#else
	kregs->nip = (unsigned long)f;
#endif
	return 0;
}
