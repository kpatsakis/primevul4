static unsigned int br_nf_post_routing(unsigned int hook, struct sk_buff *skb,
				       const struct net_device *in,
				       const struct net_device *out,
				       int (*okfn)(struct sk_buff *))
{
	struct nf_bridge_info *nf_bridge = skb->nf_bridge;
	struct net_device *realoutdev = bridge_parent(skb->dev);
	u_int8_t pf;

	if (!nf_bridge || !(nf_bridge->mask & BRNF_BRIDGED))
		return NF_ACCEPT;

	if (!realoutdev)
		return NF_DROP;

	if (skb->protocol == htons(ETH_P_IP) || IS_VLAN_IP(skb) ||
	    IS_PPPOE_IP(skb))
		pf = PF_INET;
	else if (skb->protocol == htons(ETH_P_IPV6) || IS_VLAN_IPV6(skb) ||
		 IS_PPPOE_IPV6(skb))
		pf = PF_INET6;
	else
		return NF_ACCEPT;

	/* We assume any code from br_dev_queue_push_xmit onwards doesn't care
	 * about the value of skb->pkt_type. */
	if (skb->pkt_type == PACKET_OTHERHOST) {
		skb->pkt_type = PACKET_HOST;
		nf_bridge->mask |= BRNF_PKT_TYPE;
	}

	nf_bridge_pull_encap_header(skb);
	nf_bridge_save_header(skb);
	if (pf == PF_INET)
		skb->protocol = htons(ETH_P_IP);
	else
		skb->protocol = htons(ETH_P_IPV6);

	NF_HOOK(pf, NF_INET_POST_ROUTING, skb, NULL, realoutdev,
		br_nf_dev_queue_xmit);

	return NF_STOLEN;
}
