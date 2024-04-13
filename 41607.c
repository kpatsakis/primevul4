static void __sctp_write_space(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;
	struct socket *sock = sk->sk_socket;

	if ((sctp_wspace(asoc) > 0) && sock) {
		if (waitqueue_active(&asoc->wait))
			wake_up_interruptible(&asoc->wait);

		if (sctp_writeable(sk)) {
			wait_queue_head_t *wq = sk_sleep(sk);

			if (wq && waitqueue_active(wq))
				wake_up_interruptible(wq);

			/* Note that we try to include the Async I/O support
			 * here by modeling from the current TCP/UDP code.
			 * We have not tested with it yet.
			 */
			if (!(sk->sk_shutdown & SEND_SHUTDOWN))
				sock_wake_async(sock,
						SOCK_WAKE_SPACE, POLL_OUT);
		}
	}
}
