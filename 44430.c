static inline void tm_recheckpoint_new_task(struct task_struct *new)
{
	unsigned long msr;

	if (!cpu_has_feature(CPU_FTR_TM))
		return;

	/* Recheckpoint the registers of the thread we're about to switch to.
	 *
	 * If the task was using FP, we non-lazily reload both the original and
	 * the speculative FP register states.  This is because the kernel
	 * doesn't see if/when a TM rollback occurs, so if we take an FP
	 * unavoidable later, we are unable to determine which set of FP regs
	 * need to be restored.
	 */
	if (!new->thread.regs)
		return;

	/* The TM SPRs are restored here, so that TEXASR.FS can be set
	 * before the trecheckpoint and no explosion occurs.
	 */
	tm_restore_sprs(&new->thread);

	if (!MSR_TM_ACTIVE(new->thread.regs->msr))
		return;
	msr = new->thread.tm_orig_msr;
	/* Recheckpoint to restore original checkpointed register state. */
	TM_DEBUG("*** tm_recheckpoint of pid %d "
		 "(new->msr 0x%lx, new->origmsr 0x%lx)\n",
		 new->pid, new->thread.regs->msr, msr);

	/* This loads the checkpointed FP/VEC state, if used */
	tm_recheckpoint(&new->thread, msr);

	/* This loads the speculative FP/VEC state, if used */
	if (msr & MSR_FP) {
		do_load_up_transact_fpu(&new->thread);
		new->thread.regs->msr |=
			(MSR_FP | new->thread.fpexc_mode);
	}
#ifdef CONFIG_ALTIVEC
	if (msr & MSR_VEC) {
		do_load_up_transact_altivec(&new->thread);
		new->thread.regs->msr |= MSR_VEC;
	}
#endif
	/* We may as well turn on VSX too since all the state is restored now */
	if (msr & MSR_VSX)
		new->thread.regs->msr |= MSR_VSX;

	TM_DEBUG("*** tm_recheckpoint of pid %d complete "
		 "(kernel msr 0x%lx)\n",
		 new->pid, mfmsr());
}
