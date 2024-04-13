static int tipc_sk_create(struct net *net, struct socket *sock, int protocol,
			  int kern)
{
	const struct proto_ops *ops;
	socket_state state;
	struct sock *sk;
	struct tipc_port *tp_ptr;

	/* Validate arguments */
	if (unlikely(protocol != 0))
		return -EPROTONOSUPPORT;

	switch (sock->type) {
	case SOCK_STREAM:
		ops = &stream_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_SEQPACKET:
		ops = &packet_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_DGRAM:
	case SOCK_RDM:
		ops = &msg_ops;
		state = SS_READY;
		break;
	default:
		return -EPROTOTYPE;
	}

	/* Allocate socket's protocol area */
	if (!kern)
		sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto);
	else
		sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto_kern);

	if (sk == NULL)
		return -ENOMEM;

	/* Allocate TIPC port for socket to use */
	tp_ptr = tipc_createport(sk, &dispatch, &wakeupdispatch,
				 TIPC_LOW_IMPORTANCE);
	if (unlikely(!tp_ptr)) {
		sk_free(sk);
		return -ENOMEM;
	}

	/* Finish initializing socket data structures */
	sock->ops = ops;
	sock->state = state;

	sock_init_data(sock, sk);
	sk->sk_backlog_rcv = backlog_rcv;
	sk->sk_rcvbuf = sysctl_tipc_rmem[1];
	sk->sk_data_ready = tipc_data_ready;
	sk->sk_write_space = tipc_write_space;
	tipc_sk(sk)->p = tp_ptr;
	tipc_sk(sk)->conn_timeout = CONN_TIMEOUT_DEFAULT;

	spin_unlock_bh(tp_ptr->lock);

	if (sock->state == SS_READY) {
		tipc_set_portunreturnable(tp_ptr->ref, 1);
		if (sock->type == SOCK_DGRAM)
			tipc_set_portunreliable(tp_ptr->ref, 1);
	}

	return 0;
}
