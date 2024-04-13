static u16 tsk_inc(struct tipc_sock *tsk, int msglen)
{
	if (likely(tsk->peer_caps & TIPC_BLOCK_FLOWCTL))
		return ((msglen / FLOWCTL_BLK_SZ) + 1);
	return 1;
}
