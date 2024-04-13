static void __sctp_write_space(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;

	if (sctp_wspace(asoc) <= 0)
		return;

	if (waitqueue_active(&asoc->wait))
		wake_up_interruptible(&asoc->wait);

	if (sctp_writeable(sk)) {
		struct socket_wq *wq;

		rcu_read_lock();
		wq = rcu_dereference(sk->sk_wq);
		if (wq) {
			if (waitqueue_active(&wq->wait))
				wake_up_interruptible(&wq->wait);

			/* Note that we try to include the Async I/O support
			 * here by modeling from the current TCP/UDP code.
			 * We have not tested with it yet.
			 */
			if (!(sk->sk_shutdown & SEND_SHUTDOWN))
				sock_wake_async(wq, SOCK_WAKE_SPACE, POLL_OUT);
		}
		rcu_read_unlock();
	}
}
