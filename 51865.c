static void tipc_sk_timeout(unsigned long data)
{
	struct tipc_sock *tsk = (struct tipc_sock *)data;
	struct sock *sk = &tsk->sk;
	struct sk_buff *skb = NULL;
	u32 peer_port, peer_node;
	u32 own_node = tsk_own_node(tsk);

	bh_lock_sock(sk);
	if (!tsk->connected) {
		bh_unlock_sock(sk);
		goto exit;
	}
	peer_port = tsk_peer_port(tsk);
	peer_node = tsk_peer_node(tsk);

	if (tsk->probing_state == TIPC_CONN_PROBING) {
		if (!sock_owned_by_user(sk)) {
			sk->sk_socket->state = SS_DISCONNECTING;
			tsk->connected = 0;
			tipc_node_remove_conn(sock_net(sk), tsk_peer_node(tsk),
					      tsk_peer_port(tsk));
			sk->sk_state_change(sk);
		} else {
			/* Try again later */
			sk_reset_timer(sk, &sk->sk_timer, (HZ / 20));
		}

	} else {
		skb = tipc_msg_create(CONN_MANAGER, CONN_PROBE,
				      INT_H_SIZE, 0, peer_node, own_node,
				      peer_port, tsk->portid, TIPC_OK);
		tsk->probing_state = TIPC_CONN_PROBING;
		sk_reset_timer(sk, &sk->sk_timer, jiffies + tsk->probing_intv);
	}
	bh_unlock_sock(sk);
	if (skb)
		tipc_node_xmit_skb(sock_net(sk), skb, peer_node, tsk->portid);
exit:
	sock_put(sk);
}
