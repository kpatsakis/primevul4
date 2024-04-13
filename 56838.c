COMPAT_SYSCALL_DEFINE4(mq_open, const char __user *, u_name,
		       int, oflag, compat_mode_t, mode,
		       struct compat_mq_attr __user *, u_attr)
{
	struct mq_attr attr, *p = NULL;
	if (u_attr && oflag & O_CREAT) {
		p = &attr;
		if (get_compat_mq_attr(&attr, u_attr))
			return -EFAULT;
	}
	return do_mq_open(u_name, oflag, mode, p);
}
