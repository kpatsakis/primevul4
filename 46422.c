static int is_rt_sigreturn_32_address(unsigned int nip, unsigned int fp)
{
	if (nip == fp + offsetof(struct rt_signal_frame_32,
				 uc.uc_mcontext.mc_pad))
		return 1;
	if (vdso32_rt_sigtramp && current->mm->context.vdso_base &&
	    nip == current->mm->context.vdso_base + vdso32_rt_sigtramp)
		return 1;
	return 0;
}
