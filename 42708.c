SCTP_STATIC int sctp_stream_listen(struct sock *sk, int backlog)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_endpoint *ep = sp->ep;

	/* If backlog is zero, disable listening. */
	if (!backlog) {
		if (sctp_sstate(sk, CLOSED))
			return 0;

		sctp_unhash_endpoint(ep);
		sk->sk_state = SCTP_SS_CLOSED;
	}

	if (sctp_sstate(sk, LISTENING))
		return 0;

	/*
	 * If a bind() or sctp_bindx() is not called prior to a listen()
	 * call that allows new associations to be accepted, the system
	 * picks an ephemeral port and will choose an address set equivalent
	 * to binding with a wildcard address.
	 *
	 * This is not currently spelled out in the SCTP sockets
	 * extensions draft, but follows the practice as seen in TCP
	 * sockets.
	 */
	if (!ep->base.bind_addr.port) {
		if (sctp_autobind(sk))
			return -EAGAIN;
	}
	sk->sk_state = SCTP_SS_LISTENING;
	sk->sk_max_ack_backlog = backlog;
	sctp_hash_endpoint(ep);
	return 0;
}
