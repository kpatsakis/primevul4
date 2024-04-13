static unsigned int rcvbuf_limit(struct sock *sk, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	unsigned int limit;

	if (msg_connected(msg))
		limit = sysctl_tipc_rmem[2];
	else
		limit = sk->sk_rcvbuf >> TIPC_CRITICAL_IMPORTANCE <<
			msg_importance(msg);
	return limit;
}
