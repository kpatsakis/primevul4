static int x25_create(struct net *net, struct socket *sock, int protocol,
		      int kern)
{
	struct sock *sk;
	struct x25_sock *x25;
	int rc = -EAFNOSUPPORT;

	if (!net_eq(net, &init_net))
		goto out;

	rc = -ESOCKTNOSUPPORT;
	if (sock->type != SOCK_SEQPACKET)
		goto out;

	rc = -EINVAL;
	if (protocol)
		goto out;

	rc = -ENOBUFS;
	if ((sk = x25_alloc_socket(net)) == NULL)
		goto out;

	x25 = x25_sk(sk);

	sock_init_data(sock, sk);

	x25_init_timers(sk);

	sock->ops    = &x25_proto_ops;
	sk->sk_protocol = protocol;
	sk->sk_backlog_rcv = x25_backlog_rcv;

	x25->t21   = sysctl_x25_call_request_timeout;
	x25->t22   = sysctl_x25_reset_request_timeout;
	x25->t23   = sysctl_x25_clear_request_timeout;
	x25->t2    = sysctl_x25_ack_holdback_timeout;
	x25->state = X25_STATE_0;
	x25->cudmatchlength = 0;
	set_bit(X25_ACCPT_APPRV_FLAG, &x25->flags);	/* normally no cud  */
							/* on call accept   */

	x25->facilities.winsize_in  = X25_DEFAULT_WINDOW_SIZE;
	x25->facilities.winsize_out = X25_DEFAULT_WINDOW_SIZE;
	x25->facilities.pacsize_in  = X25_DEFAULT_PACKET_SIZE;
	x25->facilities.pacsize_out = X25_DEFAULT_PACKET_SIZE;
	x25->facilities.throughput  = 0;	/* by default don't negotiate
						   throughput */
	x25->facilities.reverse     = X25_DEFAULT_REVERSE;
	x25->dte_facilities.calling_len = 0;
	x25->dte_facilities.called_len = 0;
	memset(x25->dte_facilities.called_ae, '\0',
			sizeof(x25->dte_facilities.called_ae));
	memset(x25->dte_facilities.calling_ae, '\0',
			sizeof(x25->dte_facilities.calling_ae));

	rc = 0;
out:
	return rc;
}
