static u32 tsk_own_node(struct tipc_sock *tsk)
{
	return msg_prevnode(&tsk->phdr);
}
