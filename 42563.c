static unsigned int br_nf_forward_ip(unsigned int hook, struct sk_buff *skb,
				     const struct net_device *in,
				     const struct net_device *out,
				     int (*okfn)(struct sk_buff *))
{
	struct nf_bridge_info *nf_bridge;
	struct net_device *parent;
	u_int8_t pf;

	if (!skb->nf_bridge)
		return NF_ACCEPT;

	/* Need exclusive nf_bridge_info since we might have multiple
	 * different physoutdevs. */
	if (!nf_bridge_unshare(skb))
		return NF_DROP;

	parent = bridge_parent(out);
	if (!parent)
		return NF_DROP;

	if (skb->protocol == htons(ETH_P_IP) || IS_VLAN_IP(skb) ||
	    IS_PPPOE_IP(skb))
		pf = PF_INET;
	else if (skb->protocol == htons(ETH_P_IPV6) || IS_VLAN_IPV6(skb) ||
		 IS_PPPOE_IPV6(skb))
		pf = PF_INET6;
	else
		return NF_ACCEPT;

	nf_bridge_pull_encap_header(skb);

	nf_bridge = skb->nf_bridge;
	if (skb->pkt_type == PACKET_OTHERHOST) {
		skb->pkt_type = PACKET_HOST;
		nf_bridge->mask |= BRNF_PKT_TYPE;
	}

	if (br_parse_ip_options(skb))
		return NF_DROP;

	/* The physdev module checks on this */
	nf_bridge->mask |= BRNF_BRIDGED;
	nf_bridge->physoutdev = skb->dev;
	if (pf == PF_INET)
		skb->protocol = htons(ETH_P_IP);
	else
		skb->protocol = htons(ETH_P_IPV6);

	NF_HOOK(pf, NF_INET_FORWARD, skb, bridge_parent(in), parent,
		br_nf_forward_finish);

	return NF_STOLEN;
}
