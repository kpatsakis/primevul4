bool tcp_oow_rate_limited(struct net *net, const struct sk_buff *skb,
			  int mib_idx, u32 *last_oow_ack_time)
{
	/* Data packets without SYNs are not likely part of an ACK loop. */
	if ((TCP_SKB_CB(skb)->seq != TCP_SKB_CB(skb)->end_seq) &&
	    !tcp_hdr(skb)->syn)
		goto not_rate_limited;

	if (*last_oow_ack_time) {
		s32 elapsed = (s32)(tcp_time_stamp - *last_oow_ack_time);

		if (0 <= elapsed && elapsed < sysctl_tcp_invalid_ratelimit) {
			NET_INC_STATS_BH(net, mib_idx);
			return true;	/* rate-limited: don't send yet! */
		}
	}

	*last_oow_ack_time = tcp_time_stamp;

not_rate_limited:
	return false;	/* not rate-limited: go ahead, send dupack now! */
}
