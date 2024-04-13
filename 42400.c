ip_vs_copy_stats(struct ip_vs_stats_user *dst, struct ip_vs_stats *src)
{
#define IP_VS_SHOW_STATS_COUNTER(c) dst->c = src->ustats.c - src->ustats0.c

	spin_lock_bh(&src->lock);

	IP_VS_SHOW_STATS_COUNTER(conns);
	IP_VS_SHOW_STATS_COUNTER(inpkts);
	IP_VS_SHOW_STATS_COUNTER(outpkts);
	IP_VS_SHOW_STATS_COUNTER(inbytes);
	IP_VS_SHOW_STATS_COUNTER(outbytes);

	ip_vs_read_estimator(dst, src);

	spin_unlock_bh(&src->lock);
}
