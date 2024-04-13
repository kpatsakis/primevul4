static int accept(struct socket *sock, struct socket *new_sock, int flags)
{
	struct sock *new_sk, *sk = sock->sk;
	struct sk_buff *buf;
	struct tipc_sock *new_tsock;
	struct tipc_port *new_tport;
	struct tipc_msg *msg;
	u32 new_ref;

	int res;

	lock_sock(sk);

	if (sock->state != SS_LISTENING) {
		res = -EINVAL;
		goto exit;
	}

	while (skb_queue_empty(&sk->sk_receive_queue)) {
		if (flags & O_NONBLOCK) {
			res = -EWOULDBLOCK;
			goto exit;
		}
		release_sock(sk);
		res = wait_event_interruptible(*sk_sleep(sk),
				(!skb_queue_empty(&sk->sk_receive_queue)));
		lock_sock(sk);
		if (res)
			goto exit;
	}

	buf = skb_peek(&sk->sk_receive_queue);

	res = tipc_sk_create(sock_net(sock->sk), new_sock, 0, 1);
	if (res)
		goto exit;

	new_sk = new_sock->sk;
	new_tsock = tipc_sk(new_sk);
	new_tport = new_tsock->p;
	new_ref = new_tport->ref;
	msg = buf_msg(buf);

	/* we lock on new_sk; but lockdep sees the lock on sk */
	lock_sock_nested(new_sk, SINGLE_DEPTH_NESTING);

	/*
	 * Reject any stray messages received by new socket
	 * before the socket lock was taken (very, very unlikely)
	 */
	reject_rx_queue(new_sk);

	/* Connect new socket to it's peer */
	new_tsock->peer_name.ref = msg_origport(msg);
	new_tsock->peer_name.node = msg_orignode(msg);
	tipc_connect(new_ref, &new_tsock->peer_name);
	new_sock->state = SS_CONNECTED;

	tipc_set_portimportance(new_ref, msg_importance(msg));
	if (msg_named(msg)) {
		new_tport->conn_type = msg_nametype(msg);
		new_tport->conn_instance = msg_nameinst(msg);
	}

	/*
	 * Respond to 'SYN-' by discarding it & returning 'ACK'-.
	 * Respond to 'SYN+' by queuing it on new socket.
	 */
	if (!msg_data_sz(msg)) {
		struct msghdr m = {NULL,};

		advance_rx_queue(sk);
		send_packet(NULL, new_sock, &m, 0);
	} else {
		__skb_dequeue(&sk->sk_receive_queue);
		__skb_queue_head(&new_sk->sk_receive_queue, buf);
		skb_set_owner_r(buf, new_sk);
	}
	release_sock(new_sk);

exit:
	release_sock(sk);
	return res;
}
