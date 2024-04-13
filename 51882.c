static u32 tsk_peer_port(struct tipc_sock *tsk)
{
	return msg_destport(&tsk->phdr);
}
