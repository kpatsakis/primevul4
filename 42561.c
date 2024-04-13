static unsigned int br_nf_forward_arp(unsigned int hook, struct sk_buff *skb,
				      const struct net_device *in,
				      const struct net_device *out,
				      int (*okfn)(struct sk_buff *))
{
	struct net_bridge_port *p;
	struct net_bridge *br;
	struct net_device **d = (struct net_device **)(skb->cb);

	p = br_port_get_rcu(out);
	if (p == NULL)
		return NF_ACCEPT;
	br = p->br;

	if (!brnf_call_arptables && !br->nf_call_arptables)
		return NF_ACCEPT;

	if (skb->protocol != htons(ETH_P_ARP)) {
		if (!IS_VLAN_ARP(skb))
			return NF_ACCEPT;
		nf_bridge_pull_encap_header(skb);
	}

	if (arp_hdr(skb)->ar_pln != 4) {
		if (IS_VLAN_ARP(skb))
			nf_bridge_push_encap_header(skb);
		return NF_ACCEPT;
	}
	*d = (struct net_device *)in;
	NF_HOOK(NFPROTO_ARP, NF_ARP_FORWARD, skb, (struct net_device *)in,
		(struct net_device *)out, br_nf_forward_finish);

	return NF_STOLEN;
}
