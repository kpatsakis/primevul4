static u32 tsk_peer_node(struct tipc_sock *tsk)
{
	return msg_destnode(&tsk->phdr);
}
