static int tipc_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net;
	struct tipc_sock *tsk;
	struct sk_buff *skb;
	u32 dnode;

	/*
	 * Exit if socket isn't fully initialized (occurs when a failed accept()
	 * releases a pre-allocated child socket that was never used)
	 */
	if (sk == NULL)
		return 0;

	net = sock_net(sk);
	tsk = tipc_sk(sk);
	lock_sock(sk);

	/*
	 * Reject all unreceived messages, except on an active connection
	 * (which disconnects locally & sends a 'FIN+' to peer)
	 */
	dnode = tsk_peer_node(tsk);
	while (sock->state != SS_DISCONNECTING) {
		skb = __skb_dequeue(&sk->sk_receive_queue);
		if (skb == NULL)
			break;
		if (TIPC_SKB_CB(skb)->handle != NULL)
			kfree_skb(skb);
		else {
			if ((sock->state == SS_CONNECTING) ||
			    (sock->state == SS_CONNECTED)) {
				sock->state = SS_DISCONNECTING;
				tsk->connected = 0;
				tipc_node_remove_conn(net, dnode, tsk->portid);
			}
			tipc_sk_respond(sk, skb, TIPC_ERR_NO_PORT);
		}
	}

	tipc_sk_withdraw(tsk, 0, NULL);
	sk_stop_timer(sk, &sk->sk_timer);
	tipc_sk_remove(tsk);
	if (tsk->connected) {
		skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
				      TIPC_CONN_MSG, SHORT_H_SIZE, 0, dnode,
				      tsk_own_node(tsk), tsk_peer_port(tsk),
				      tsk->portid, TIPC_ERR_NO_PORT);
		if (skb)
			tipc_node_xmit_skb(net, skb, dnode, tsk->portid);
		tipc_node_remove_conn(net, dnode, tsk->portid);
	}

	/* Reject any messages that accumulated in backlog queue */
	sock->state = SS_DISCONNECTING;
	release_sock(sk);

	call_rcu(&tsk->rcu, tipc_sk_callback);
	sock->sk = NULL;

	return 0;
}
