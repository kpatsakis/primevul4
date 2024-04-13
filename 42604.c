static int apparmor_ptrace_traceme(struct task_struct *parent)
{
	int error = cap_ptrace_traceme(parent);
	if (error)
		return error;

	return aa_ptrace(parent, current, PTRACE_MODE_ATTACH);
}
