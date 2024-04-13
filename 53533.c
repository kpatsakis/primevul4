void restore_tm_state(struct pt_regs *regs)
{
	unsigned long msr_diff;

	clear_thread_flag(TIF_RESTORE_TM);
	if (!MSR_TM_ACTIVE(regs->msr))
		return;

	msr_diff = current->thread.ckpt_regs.msr & ~regs->msr;
	msr_diff &= MSR_FP | MSR_VEC | MSR_VSX;
	if (msr_diff & MSR_FP) {
		fp_enable();
		load_fp_state(&current->thread.fp_state);
		regs->msr |= current->thread.fpexc_mode;
	}
	if (msr_diff & MSR_VEC) {
		vec_enable();
		load_vr_state(&current->thread.vr_state);
	}
	regs->msr |= msr_diff;
}
