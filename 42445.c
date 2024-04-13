ip_vs_zero_stats(struct ip_vs_stats *stats)
{
	spin_lock_bh(&stats->lock);

	/* get current counters as zero point, rates are zeroed */

#define IP_VS_ZERO_STATS_COUNTER(c) stats->ustats0.c = stats->ustats.c

	IP_VS_ZERO_STATS_COUNTER(conns);
	IP_VS_ZERO_STATS_COUNTER(inpkts);
	IP_VS_ZERO_STATS_COUNTER(outpkts);
	IP_VS_ZERO_STATS_COUNTER(inbytes);
	IP_VS_ZERO_STATS_COUNTER(outbytes);

	ip_vs_zero_estimator(stats);

	spin_unlock_bh(&stats->lock);
}
