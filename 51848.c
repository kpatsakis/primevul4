static int tipc_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *skb;
	u32 dnode = tsk_peer_node(tsk);
	u32 dport = tsk_peer_port(tsk);
	u32 onode = tipc_own_addr(net);
	u32 oport = tsk->portid;
	int res;

	if (how != SHUT_RDWR)
		return -EINVAL;

	lock_sock(sk);

	switch (sock->state) {
	case SS_CONNECTING:
	case SS_CONNECTED:

restart:
		dnode = tsk_peer_node(tsk);

		/* Disconnect and send a 'FIN+' or 'FIN-' message to peer */
		skb = __skb_dequeue(&sk->sk_receive_queue);
		if (skb) {
			if (TIPC_SKB_CB(skb)->handle != NULL) {
				kfree_skb(skb);
				goto restart;
			}
			tipc_sk_respond(sk, skb, TIPC_CONN_SHUTDOWN);
		} else {
			skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
					      TIPC_CONN_MSG, SHORT_H_SIZE,
					      0, dnode, onode, dport, oport,
					      TIPC_CONN_SHUTDOWN);
			tipc_node_xmit_skb(net, skb, dnode, tsk->portid);
		}
		tsk->connected = 0;
		sock->state = SS_DISCONNECTING;
		tipc_node_remove_conn(net, dnode, tsk->portid);
		/* fall through */

	case SS_DISCONNECTING:

		/* Discard any unreceived messages */
		__skb_queue_purge(&sk->sk_receive_queue);

		/* Wake up anyone sleeping in poll */
		sk->sk_state_change(sk);
		res = 0;
		break;

	default:
		res = -ENOTCONN;
	}

	release_sock(sk);
	return res;
}
