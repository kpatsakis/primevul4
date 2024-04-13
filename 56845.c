SYSCALL_DEFINE5(mq_timedsend, mqd_t, mqdes, const char __user *, u_msg_ptr,
		size_t, msg_len, unsigned int, msg_prio,
		const struct timespec __user *, u_abs_timeout)
{
	struct timespec ts, *p = NULL;
	if (u_abs_timeout) {
		int res = prepare_timeout(u_abs_timeout, &ts);
		if (res)
			return res;
		p = &ts;
	}
	return do_mq_timedsend(mqdes, u_msg_ptr, msg_len, msg_prio, p);
}
