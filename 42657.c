SCTP_STATIC int sctp_do_peeloff(struct sctp_association *asoc,
				struct socket **sockp)
{
	struct sock *sk = asoc->base.sk;
	struct socket *sock;
	struct inet_sock *inetsk;
	int err = 0;

	/* An association cannot be branched off from an already peeled-off
	 * socket, nor is this supported for tcp style sockets.
	 */
	if (!sctp_style(sk, UDP))
		return -EINVAL;

	/* Create a new socket.  */
	err = sock_create(sk->sk_family, SOCK_SEQPACKET, IPPROTO_SCTP, &sock);
	if (err < 0)
		return err;

	/* Populate the fields of the newsk from the oldsk and migrate the
	 * asoc to the newsk.
	 */
	sctp_sock_migrate(sk, sock->sk, asoc, SCTP_SOCKET_UDP_HIGH_BANDWIDTH);

	/* Make peeled-off sockets more like 1-1 accepted sockets.
	 * Set the daddr and initialize id to something more random
	 */
	inetsk = inet_sk(sock->sk);
	inetsk->daddr = asoc->peer.primary_addr.v4.sin_addr.s_addr;
	inetsk->id = asoc->next_tsn ^ jiffies;

	*sockp = sock;

	return err;
}
