static int is_sigreturn_32_address(unsigned int nip, unsigned int fp)
{
	if (nip == fp + offsetof(struct signal_frame_32, mctx.mc_pad))
		return 1;
	if (vdso32_sigtramp && current->mm->context.vdso_base &&
	    nip == current->mm->context.vdso_base + vdso32_sigtramp)
		return 1;
	return 0;
}
