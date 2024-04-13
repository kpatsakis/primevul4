static int tsk_importance(struct tipc_sock *tsk)
{
	return msg_importance(&tsk->phdr);
}
