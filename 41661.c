int sctp_inet_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	struct sctp_endpoint *ep = sctp_sk(sk)->ep;
	int err = -EINVAL;

	if (unlikely(backlog < 0))
		return err;

	sctp_lock_sock(sk);

	/* Peeled-off sockets are not allowed to listen().  */
	if (sctp_style(sk, UDP_HIGH_BANDWIDTH))
		goto out;

	if (sock->state != SS_UNCONNECTED)
		goto out;

	/* If backlog is zero, disable listening. */
	if (!backlog) {
		if (sctp_sstate(sk, CLOSED))
			goto out;

		err = 0;
		sctp_unhash_endpoint(ep);
		sk->sk_state = SCTP_SS_CLOSED;
		if (sk->sk_reuse)
			sctp_sk(sk)->bind_hash->fastreuse = 1;
		goto out;
	}

	/* If we are already listening, just update the backlog */
	if (sctp_sstate(sk, LISTENING))
		sk->sk_max_ack_backlog = backlog;
	else {
		err = sctp_listen_start(sk, backlog);
		if (err)
			goto out;
	}

	err = 0;
out:
	sctp_release_sock(sk);
	return err;
}
