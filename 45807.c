static int x25_wait_for_connection_establishment(struct sock *sk)
{
	DECLARE_WAITQUEUE(wait, current);
	int rc;

	add_wait_queue_exclusive(sk_sleep(sk), &wait);
	for (;;) {
		__set_current_state(TASK_INTERRUPTIBLE);
		rc = -ERESTARTSYS;
		if (signal_pending(current))
			break;
		rc = sock_error(sk);
		if (rc) {
			sk->sk_socket->state = SS_UNCONNECTED;
			break;
		}
		rc = 0;
		if (sk->sk_state != TCP_ESTABLISHED) {
			release_sock(sk);
			schedule();
			lock_sock(sk);
		} else
			break;
	}
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(sk), &wait);
	return rc;
}
