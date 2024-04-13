static unsigned int br_nf_pre_routing(unsigned int hook, struct sk_buff *skb,
				      const struct net_device *in,
				      const struct net_device *out,
				      int (*okfn)(struct sk_buff *))
{
	struct net_bridge_port *p;
	struct net_bridge *br;
	__u32 len = nf_bridge_encap_header_len(skb);

	if (unlikely(!pskb_may_pull(skb, len)))
		return NF_DROP;

	p = br_port_get_rcu(in);
	if (p == NULL)
		return NF_DROP;
	br = p->br;

	if (skb->protocol == htons(ETH_P_IPV6) || IS_VLAN_IPV6(skb) ||
	    IS_PPPOE_IPV6(skb)) {
		if (!brnf_call_ip6tables && !br->nf_call_ip6tables)
			return NF_ACCEPT;

		nf_bridge_pull_encap_header_rcsum(skb);
		return br_nf_pre_routing_ipv6(hook, skb, in, out, okfn);
	}

	if (!brnf_call_iptables && !br->nf_call_iptables)
		return NF_ACCEPT;

	if (skb->protocol != htons(ETH_P_IP) && !IS_VLAN_IP(skb) &&
	    !IS_PPPOE_IP(skb))
		return NF_ACCEPT;

	nf_bridge_pull_encap_header_rcsum(skb);

	if (br_parse_ip_options(skb))
		return NF_DROP;

	nf_bridge_put(skb->nf_bridge);
	if (!nf_bridge_alloc(skb))
		return NF_DROP;
	if (!setup_pre_routing(skb))
		return NF_DROP;
	store_orig_dstaddr(skb);
	skb->protocol = htons(ETH_P_IP);

	NF_HOOK(NFPROTO_IPV4, NF_INET_PRE_ROUTING, skb, skb->dev, NULL,
		br_nf_pre_routing_finish);

	return NF_STOLEN;
}
