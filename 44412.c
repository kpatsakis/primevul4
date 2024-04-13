int get_fpexc_mode(struct task_struct *tsk, unsigned long adr)
{
	unsigned int val;

	if (tsk->thread.fpexc_mode & PR_FP_EXC_SW_ENABLE)
#ifdef CONFIG_SPE
		if (cpu_has_feature(CPU_FTR_SPE)) {
			/*
			 * When the sticky exception bits are set
			 * directly by userspace, it must call prctl
			 * with PR_GET_FPEXC (with PR_FP_EXC_SW_ENABLE
			 * in the existing prctl settings) or
			 * PR_SET_FPEXC (with PR_FP_EXC_SW_ENABLE in
			 * the bits being set).  <fenv.h> functions
			 * saving and restoring the whole
			 * floating-point environment need to do so
			 * anyway to restore the prctl settings from
			 * the saved environment.
			 */
			tsk->thread.spefscr_last = mfspr(SPRN_SPEFSCR);
			val = tsk->thread.fpexc_mode;
		} else
			return -EINVAL;
#else
		return -EINVAL;
#endif
	else
		val = __unpack_fe01(tsk->thread.fpexc_mode);
	return put_user(val, (unsigned int __user *) adr);
}
