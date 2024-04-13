COMPAT_SYSCALL_DEFINE4(rt_tgsigqueueinfo,
			compat_pid_t, tgid,
			compat_pid_t, pid,
			int, sig,
			struct compat_siginfo __user *, uinfo)
{
	siginfo_t info;

	if (copy_siginfo_from_user32(&info, uinfo))
		return -EFAULT;
	return do_rt_tgsigqueueinfo(tgid, pid, sig, &info);
}
