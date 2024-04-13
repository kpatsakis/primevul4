static __u32 tpacket_get_timestamp(struct sk_buff *skb, struct timespec *ts,
				   unsigned int flags)
{
	struct skb_shared_hwtstamps *shhwtstamps = skb_hwtstamps(skb);

	if (shhwtstamps &&
	    (flags & SOF_TIMESTAMPING_RAW_HARDWARE) &&
	    ktime_to_timespec_cond(shhwtstamps->hwtstamp, ts))
		return TP_STATUS_TS_RAW_HARDWARE;

	if (ktime_to_timespec_cond(skb->tstamp, ts))
		return TP_STATUS_TS_SOFTWARE;

	return 0;
}
