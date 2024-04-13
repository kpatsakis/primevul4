SYSCALL_DEFINE2(mq_notify, mqd_t, mqdes,
		const struct sigevent __user *, u_notification)
{
	struct sigevent n, *p = NULL;
	if (u_notification) {
		if (copy_from_user(&n, u_notification, sizeof(struct sigevent)))
			return -EFAULT;
		p = &n;
	}
	return do_mq_notify(mqdes, p);
}
