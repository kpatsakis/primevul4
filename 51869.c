static int tipc_wait_for_connect(struct socket *sock, long *timeo_p)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT(wait);
	int done;

	do {
		int err = sock_error(sk);
		if (err)
			return err;
		if (!*timeo_p)
			return -ETIMEDOUT;
		if (signal_pending(current))
			return sock_intr_errno(*timeo_p);

		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		done = sk_wait_event(sk, timeo_p, sock->state != SS_CONNECTING);
		finish_wait(sk_sleep(sk), &wait);
	} while (!done);
	return 0;
}
