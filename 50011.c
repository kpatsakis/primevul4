static void mip6_addr_swap(struct sk_buff *skb)
{
	struct ipv6hdr *iph = ipv6_hdr(skb);
	struct inet6_skb_parm *opt = IP6CB(skb);
	struct ipv6_destopt_hao *hao;
	struct in6_addr tmp;
	int off;

	if (opt->dsthao) {
		off = ipv6_find_tlv(skb, opt->dsthao, IPV6_TLV_HAO);
		if (likely(off >= 0)) {
			hao = (struct ipv6_destopt_hao *)
					(skb_network_header(skb) + off);
			tmp = iph->saddr;
			iph->saddr = hao->addr;
			hao->addr = tmp;
		}
	}
}
