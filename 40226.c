static int ptrace_setoptions(struct task_struct *child, unsigned long data)
{
	unsigned flags;

	if (data & ~(unsigned long)PTRACE_O_MASK)
		return -EINVAL;

	/* Avoid intermediate state when all opts are cleared */
	flags = child->ptrace;
	flags &= ~(PTRACE_O_MASK << PT_OPT_FLAG_SHIFT);
	flags |= (data << PT_OPT_FLAG_SHIFT);
	child->ptrace = flags;

	return 0;
}
