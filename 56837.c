COMPAT_SYSCALL_DEFINE3(mq_getsetattr, mqd_t, mqdes,
		       const struct compat_mq_attr __user *, u_mqstat,
		       struct compat_mq_attr __user *, u_omqstat)
{
	int ret;
	struct mq_attr mqstat, omqstat;
	struct mq_attr *new = NULL, *old = NULL;

	if (u_mqstat) {
		new = &mqstat;
		if (get_compat_mq_attr(new, u_mqstat))
			return -EFAULT;
	}
	if (u_omqstat)
		old = &omqstat;

	ret = do_mq_getsetattr(mqdes, new, old);
	if (ret || !old)
		return ret;

	if (put_compat_mq_attr(old, u_omqstat))
		return -EFAULT;
	return 0;
}
