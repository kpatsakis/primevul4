static int tsk_set_importance(struct tipc_sock *tsk, int imp)
{
	if (imp > TIPC_CRITICAL_IMPORTANCE)
		return -EINVAL;
	msg_set_importance(&tsk->phdr, (u32)imp);
	return 0;
}
