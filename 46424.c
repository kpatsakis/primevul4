static int is_sigreturn_64_address(unsigned long nip, unsigned long fp)
{
	if (nip == fp + offsetof(struct signal_frame_64, tramp))
		return 1;
	if (vdso64_rt_sigtramp && current->mm->context.vdso_base &&
	    nip == current->mm->context.vdso_base + vdso64_rt_sigtramp)
		return 1;
	return 0;
}
