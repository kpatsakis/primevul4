static int ptrace_has_cap(struct user_namespace *ns, unsigned int mode)
{
	if (mode & PTRACE_MODE_NOAUDIT)
		return has_ns_capability_noaudit(current, ns, CAP_SYS_PTRACE);
	else
		return has_ns_capability(current, ns, CAP_SYS_PTRACE);
}
