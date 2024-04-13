static inline void net_timestamp_check(struct sk_buff *skb)
{
	if (!skb->tstamp.tv64 && atomic_read(&netstamp_needed))
		__net_timestamp(skb);
}
