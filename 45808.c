static int x25_wait_for_data(struct sock *sk, long timeout)
{
	DECLARE_WAITQUEUE(wait, current);
	int rc = 0;

	add_wait_queue_exclusive(sk_sleep(sk), &wait);
	for (;;) {
		__set_current_state(TASK_INTERRUPTIBLE);
		if (sk->sk_shutdown & RCV_SHUTDOWN)
			break;
		rc = -ERESTARTSYS;
		if (signal_pending(current))
			break;
		rc = -EAGAIN;
		if (!timeout)
			break;
		rc = 0;
		if (skb_queue_empty(&sk->sk_receive_queue)) {
			release_sock(sk);
			timeout = schedule_timeout(timeout);
			lock_sock(sk);
		} else
			break;
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(sk), &wait);
	return rc;
}
