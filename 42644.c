static void __sctp_write_space(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;
	struct socket *sock = sk->sk_socket;

	if ((sctp_wspace(asoc) > 0) && sock) {
		if (waitqueue_active(&asoc->wait))
			wake_up_interruptible(&asoc->wait);

		if (sctp_writeable(sk)) {
			if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
				wake_up_interruptible(sk->sk_sleep);

			/* Note that we try to include the Async I/O support
			 * here by modeling from the current TCP/UDP code.
			 * We have not tested with it yet.
			 */
			if (sock->fasync_list &&
			    !(sk->sk_shutdown & SEND_SHUTDOWN))
				sock_wake_async(sock, 2, POLL_OUT);
		}
	}
}
