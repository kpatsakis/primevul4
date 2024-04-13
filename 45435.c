int bt_sock_wait_ready(struct sock *sk, unsigned long flags)
{
	DECLARE_WAITQUEUE(wait, current);
	unsigned long timeo;
	int err = 0;

	BT_DBG("sk %p", sk);

	timeo = sock_sndtimeo(sk, flags & O_NONBLOCK);

	add_wait_queue(sk_sleep(sk), &wait);
	set_current_state(TASK_INTERRUPTIBLE);
	while (test_bit(BT_SK_SUSPEND, &bt_sk(sk)->flags)) {
		if (!timeo) {
			err = -EAGAIN;
			break;
		}

		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			break;
		}

		release_sock(sk);
		timeo = schedule_timeout(timeo);
		lock_sock(sk);
		set_current_state(TASK_INTERRUPTIBLE);

		err = sock_error(sk);
		if (err)
			break;
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(sk), &wait);

	return err;
}
