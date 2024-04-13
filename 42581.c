static struct net_device *setup_pre_routing(struct sk_buff *skb)
{
	struct nf_bridge_info *nf_bridge = skb->nf_bridge;

	if (skb->pkt_type == PACKET_OTHERHOST) {
		skb->pkt_type = PACKET_HOST;
		nf_bridge->mask |= BRNF_PKT_TYPE;
	}

	nf_bridge->mask |= BRNF_NF_BRIDGE_PREROUTING;
	nf_bridge->physindev = skb->dev;
	skb->dev = bridge_parent(skb->dev);
	if (skb->protocol == htons(ETH_P_8021Q))
		nf_bridge->mask |= BRNF_8021Q;
	else if (skb->protocol == htons(ETH_P_PPP_SES))
		nf_bridge->mask |= BRNF_PPPoE;

	return skb->dev;
}
