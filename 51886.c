static void tsk_set_unreturnable(struct tipc_sock *tsk, bool unreturnable)
{
	msg_set_dest_droppable(&tsk->phdr, unreturnable ? 1 : 0);
}
