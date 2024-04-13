COMPAT_SYSCALL_DEFINE2(mq_notify, mqd_t, mqdes,
		       const struct compat_sigevent __user *, u_notification)
{
	struct sigevent n, *p = NULL;
	if (u_notification) {
		if (get_compat_sigevent(&n, u_notification))
			return -EFAULT;
		if (n.sigev_notify == SIGEV_THREAD)
			n.sigev_value.sival_ptr = compat_ptr(n.sigev_value.sival_int);
		p = &n;
	}
	return do_mq_notify(mqdes, p);
}
