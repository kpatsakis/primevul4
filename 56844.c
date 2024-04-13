SYSCALL_DEFINE4(mq_open, const char __user *, u_name, int, oflag, umode_t, mode,
		struct mq_attr __user *, u_attr)
{
	struct mq_attr attr;
	if (u_attr && copy_from_user(&attr, u_attr, sizeof(struct mq_attr)))
		return -EFAULT;

	return do_mq_open(u_name, oflag, mode, u_attr ? &attr : NULL);
}
