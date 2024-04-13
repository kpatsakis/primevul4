static int sane_signal_32_frame(unsigned int sp)
{
	struct signal_frame_32 __user *sf;
	unsigned int regs;

	sf = (struct signal_frame_32 __user *) (unsigned long) sp;
	if (read_user_stack_32((unsigned int __user *) &sf->sctx.regs, &regs))
		return 0;
	return regs == (unsigned long) &sf->mctx;
}
