static void tcp_rcv_nxt_update(struct tcp_sock *tp, u32 seq)
{
	u32 delta = seq - tp->rcv_nxt;

	u64_stats_update_begin(&tp->syncp);
	tp->bytes_received += delta;
	u64_stats_update_end(&tp->syncp);
	tp->rcv_nxt = seq;
}
