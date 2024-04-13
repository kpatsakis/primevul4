static void tipc_sk_finish_conn(struct tipc_sock *tsk, u32 peer_port,
				u32 peer_node)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct tipc_msg *msg = &tsk->phdr;

	msg_set_destnode(msg, peer_node);
	msg_set_destport(msg, peer_port);
	msg_set_type(msg, TIPC_CONN_MSG);
	msg_set_lookup_scope(msg, 0);
	msg_set_hdr_sz(msg, SHORT_H_SIZE);

	tsk->probing_intv = CONN_PROBING_INTERVAL;
	tsk->probing_state = TIPC_CONN_OK;
	tsk->connected = 1;
	sk_reset_timer(sk, &sk->sk_timer, jiffies + tsk->probing_intv);
	tipc_node_add_conn(net, peer_node, tsk->portid, peer_port);
	tsk->max_pkt = tipc_node_get_mtu(net, peer_node, tsk->portid);
	tsk->peer_caps = tipc_node_get_capabilities(net, peer_node);
	if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL)
		return;

	/* Fall back to message based flow control */
	tsk->rcv_win = FLOWCTL_MSG_WIN;
	tsk->snd_win = FLOWCTL_MSG_WIN;
}
