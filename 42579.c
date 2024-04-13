static inline void nf_bridge_update_protocol(struct sk_buff *skb)
{
	if (skb->nf_bridge->mask & BRNF_8021Q)
		skb->protocol = htons(ETH_P_8021Q);
	else if (skb->nf_bridge->mask & BRNF_PPPoE)
		skb->protocol = htons(ETH_P_PPP_SES);
}
