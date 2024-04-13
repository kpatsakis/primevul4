int tcp_abort(struct sock *sk, int err)
{
	if (!sk_fullsock(sk)) {
		if (sk->sk_state == TCP_NEW_SYN_RECV) {
			struct request_sock *req = inet_reqsk(sk);

			local_bh_disable();
			inet_csk_reqsk_queue_drop_and_put(req->rsk_listener,
							  req);
			local_bh_enable();
			return 0;
		}
		return -EOPNOTSUPP;
	}

	/* Don't race with userspace socket closes such as tcp_close. */
	lock_sock(sk);

	if (sk->sk_state == TCP_LISTEN) {
		tcp_set_state(sk, TCP_CLOSE);
		inet_csk_listen_stop(sk);
	}

	/* Don't race with BH socket closes such as inet_csk_listen_stop. */
	local_bh_disable();
	bh_lock_sock(sk);

	if (!sock_flag(sk, SOCK_DEAD)) {
		sk->sk_err = err;
		/* This barrier is coupled with smp_rmb() in tcp_poll() */
		smp_wmb();
		sk->sk_error_report(sk);
		if (tcp_need_reset(sk->sk_state))
			tcp_send_active_reset(sk, GFP_ATOMIC);
		tcp_done(sk);
	}

	bh_unlock_sock(sk);
	local_bh_enable();
	release_sock(sk);
	return 0;
}
