static bool tsk_conn_cong(struct tipc_sock *tsk)
{
	return tsk->snt_unacked >= tsk->snd_win;
}
