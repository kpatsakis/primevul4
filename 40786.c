SYSCALL_DEFINE3(rt_sigqueueinfo, pid_t, pid, int, sig,
		siginfo_t __user *, uinfo)
{
	siginfo_t info;
	if (copy_from_user(&info, uinfo, sizeof(siginfo_t)))
		return -EFAULT;
	return do_rt_sigqueueinfo(pid, sig, &info);
}
