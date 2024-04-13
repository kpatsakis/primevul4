static bool tsk_peer_msg(struct tipc_sock *tsk, struct tipc_msg *msg)
{
	struct tipc_net *tn = net_generic(sock_net(&tsk->sk), tipc_net_id);
	u32 peer_port = tsk_peer_port(tsk);
	u32 orig_node;
	u32 peer_node;

	if (unlikely(!tsk->connected))
		return false;

	if (unlikely(msg_origport(msg) != peer_port))
		return false;

	orig_node = msg_orignode(msg);
	peer_node = tsk_peer_node(tsk);

	if (likely(orig_node == peer_node))
		return true;

	if (!orig_node && (peer_node == tn->own_addr))
		return true;

	if (!peer_node && (orig_node == tn->own_addr))
		return true;

	return false;
}
