static int br_nf_dev_queue_xmit(struct sk_buff *skb)
{
	int ret;

	if (skb->nfct != NULL && skb->protocol == htons(ETH_P_IP) &&
	    skb->len + nf_bridge_mtu_reduction(skb) > skb->dev->mtu &&
	    !skb_is_gso(skb)) {
		if (br_parse_ip_options(skb))
			/* Drop invalid packet */
			return NF_DROP;
		ret = ip_fragment(skb, br_dev_queue_push_xmit);
	} else
		ret = br_dev_queue_push_xmit(skb);

	return ret;
}
