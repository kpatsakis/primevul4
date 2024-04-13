struct sk_buff *tcp_get_timestamping_opt_stats(const struct sock *sk)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	struct sk_buff *stats;
	struct tcp_info info;

	stats = alloc_skb(3 * nla_total_size_64bit(sizeof(u64)), GFP_ATOMIC);
	if (!stats)
		return NULL;

	tcp_get_info_chrono_stats(tp, &info);
	nla_put_u64_64bit(stats, TCP_NLA_BUSY,
			  info.tcpi_busy_time, TCP_NLA_PAD);
	nla_put_u64_64bit(stats, TCP_NLA_RWND_LIMITED,
			  info.tcpi_rwnd_limited, TCP_NLA_PAD);
	nla_put_u64_64bit(stats, TCP_NLA_SNDBUF_LIMITED,
			  info.tcpi_sndbuf_limited, TCP_NLA_PAD);
	return stats;
}
