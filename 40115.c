static void vcc_write_space(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();

	if (vcc_writable(sk)) {
		wq = rcu_dereference(sk->sk_wq);
		if (wq_has_sleeper(wq))
			wake_up_interruptible(&wq->wait);

		sk_wake_async(sk, SOCK_WAKE_SPACE, POLL_OUT);
	}

	rcu_read_unlock();
}
