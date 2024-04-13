void skb_tstamp_tx(struct sk_buff *orig_skb,
		   struct skb_shared_hwtstamps *hwtstamps)
{
	return __skb_tstamp_tx(orig_skb, hwtstamps, orig_skb->sk,
			       SCM_TSTAMP_SND);
}
