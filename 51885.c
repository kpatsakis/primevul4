static void tsk_set_unreliable(struct tipc_sock *tsk, bool unreliable)
{
	msg_set_src_droppable(&tsk->phdr, unreliable ? 1 : 0);
}
