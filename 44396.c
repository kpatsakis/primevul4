struct task_struct *__switch_to(struct task_struct *prev,
	struct task_struct *new)
{
	struct thread_struct *new_thread, *old_thread;
	struct task_struct *last;
#ifdef CONFIG_PPC_BOOK3S_64
	struct ppc64_tlb_batch *batch;
#endif

	WARN_ON(!irqs_disabled());

	/* Back up the TAR across context switches.
	 * Note that the TAR is not available for use in the kernel.  (To
	 * provide this, the TAR should be backed up/restored on exception
	 * entry/exit instead, and be in pt_regs.  FIXME, this should be in
	 * pt_regs anyway (for debug).)
	 * Save the TAR here before we do treclaim/trecheckpoint as these
	 * will change the TAR.
	 */
	save_tar(&prev->thread);

	__switch_to_tm(prev);

#ifdef CONFIG_SMP
	/* avoid complexity of lazy save/restore of fpu
	 * by just saving it every time we switch out if
	 * this task used the fpu during the last quantum.
	 *
	 * If it tries to use the fpu again, it'll trap and
	 * reload its fp regs.  So we don't have to do a restore
	 * every switch, just a save.
	 *  -- Cort
	 */
	if (prev->thread.regs && (prev->thread.regs->msr & MSR_FP))
		giveup_fpu(prev);
#ifdef CONFIG_ALTIVEC
	/*
	 * If the previous thread used altivec in the last quantum
	 * (thus changing altivec regs) then save them.
	 * We used to check the VRSAVE register but not all apps
	 * set it, so we don't rely on it now (and in fact we need
	 * to save & restore VSCR even if VRSAVE == 0).  -- paulus
	 *
	 * On SMP we always save/restore altivec regs just to avoid the
	 * complexity of changing processors.
	 *  -- Cort
	 */
	if (prev->thread.regs && (prev->thread.regs->msr & MSR_VEC))
		giveup_altivec(prev);
#endif /* CONFIG_ALTIVEC */
#ifdef CONFIG_VSX
	if (prev->thread.regs && (prev->thread.regs->msr & MSR_VSX))
		/* VMX and FPU registers are already save here */
		__giveup_vsx(prev);
#endif /* CONFIG_VSX */
#ifdef CONFIG_SPE
	/*
	 * If the previous thread used spe in the last quantum
	 * (thus changing spe regs) then save them.
	 *
	 * On SMP we always save/restore spe regs just to avoid the
	 * complexity of changing processors.
	 */
	if ((prev->thread.regs && (prev->thread.regs->msr & MSR_SPE)))
		giveup_spe(prev);
#endif /* CONFIG_SPE */

#else  /* CONFIG_SMP */
#ifdef CONFIG_ALTIVEC
	/* Avoid the trap.  On smp this this never happens since
	 * we don't set last_task_used_altivec -- Cort
	 */
	if (new->thread.regs && last_task_used_altivec == new)
		new->thread.regs->msr |= MSR_VEC;
#endif /* CONFIG_ALTIVEC */
#ifdef CONFIG_VSX
	if (new->thread.regs && last_task_used_vsx == new)
		new->thread.regs->msr |= MSR_VSX;
#endif /* CONFIG_VSX */
#ifdef CONFIG_SPE
	/* Avoid the trap.  On smp this this never happens since
	 * we don't set last_task_used_spe
	 */
	if (new->thread.regs && last_task_used_spe == new)
		new->thread.regs->msr |= MSR_SPE;
#endif /* CONFIG_SPE */

#endif /* CONFIG_SMP */

#ifdef CONFIG_PPC_ADV_DEBUG_REGS
	switch_booke_debug_regs(&new->thread.debug);
#else
/*
 * For PPC_BOOK3S_64, we use the hw-breakpoint interfaces that would
 * schedule DABR
 */
#ifndef CONFIG_HAVE_HW_BREAKPOINT
	if (unlikely(!hw_brk_match(&__get_cpu_var(current_brk), &new->thread.hw_brk)))
		set_breakpoint(&new->thread.hw_brk);
#endif /* CONFIG_HAVE_HW_BREAKPOINT */
#endif


	new_thread = &new->thread;
	old_thread = &current->thread;

#ifdef CONFIG_PPC64
	/*
	 * Collect processor utilization data per process
	 */
	if (firmware_has_feature(FW_FEATURE_SPLPAR)) {
		struct cpu_usage *cu = &__get_cpu_var(cpu_usage_array);
		long unsigned start_tb, current_tb;
		start_tb = old_thread->start_tb;
		cu->current_tb = current_tb = mfspr(SPRN_PURR);
		old_thread->accum_tb += (current_tb - start_tb);
		new_thread->start_tb = current_tb;
	}
#endif /* CONFIG_PPC64 */

#ifdef CONFIG_PPC_BOOK3S_64
	batch = &__get_cpu_var(ppc64_tlb_batch);
	if (batch->active) {
		current_thread_info()->local_flags |= _TLF_LAZY_MMU;
		if (batch->index)
			__flush_tlb_pending(batch);
		batch->active = 0;
	}
#endif /* CONFIG_PPC_BOOK3S_64 */

	/*
	 * We can't take a PMU exception inside _switch() since there is a
	 * window where the kernel stack SLB and the kernel stack are out
	 * of sync. Hard disable here.
	 */
	hard_irq_disable();

	tm_recheckpoint_new_task(new);

	last = _switch(old_thread, new_thread);

#ifdef CONFIG_PPC_BOOK3S_64
	if (current_thread_info()->local_flags & _TLF_LAZY_MMU) {
		current_thread_info()->local_flags &= ~_TLF_LAZY_MMU;
		batch = &__get_cpu_var(ppc64_tlb_batch);
		batch->active = 1;
	}
#endif /* CONFIG_PPC_BOOK3S_64 */

	return last;
}
