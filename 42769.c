__u32 __skb_get_rxhash(struct sk_buff *skb)
{
	int nhoff, hash = 0, poff;
	struct ipv6hdr *ip6;
	struct iphdr *ip;
	u8 ip_proto;
	u32 addr1, addr2, ihl;
	union {
		u32 v32;
		u16 v16[2];
	} ports;

	nhoff = skb_network_offset(skb);

	switch (skb->protocol) {
	case __constant_htons(ETH_P_IP):
		if (!pskb_may_pull(skb, sizeof(*ip) + nhoff))
			goto done;

		ip = (struct iphdr *) (skb->data + nhoff);
		if (ip->frag_off & htons(IP_MF | IP_OFFSET))
			ip_proto = 0;
		else
			ip_proto = ip->protocol;
		addr1 = (__force u32) ip->saddr;
		addr2 = (__force u32) ip->daddr;
		ihl = ip->ihl;
		break;
	case __constant_htons(ETH_P_IPV6):
		if (!pskb_may_pull(skb, sizeof(*ip6) + nhoff))
			goto done;

		ip6 = (struct ipv6hdr *) (skb->data + nhoff);
		ip_proto = ip6->nexthdr;
		addr1 = (__force u32) ip6->saddr.s6_addr32[3];
		addr2 = (__force u32) ip6->daddr.s6_addr32[3];
		ihl = (40 >> 2);
		break;
	default:
		goto done;
	}

	ports.v32 = 0;
	poff = proto_ports_offset(ip_proto);
	if (poff >= 0) {
		nhoff += ihl * 4 + poff;
		if (pskb_may_pull(skb, nhoff + 4)) {
			ports.v32 = * (__force u32 *) (skb->data + nhoff);
			if (ports.v16[1] < ports.v16[0])
				swap(ports.v16[0], ports.v16[1]);
		}
	}

	/* get a consistent hash (same value on both flow directions) */
	if (addr2 < addr1)
		swap(addr1, addr2);

	hash = jhash_3words(addr1, addr2, ports.v32, hashrnd);
	if (!hash)
		hash = 1;

done:
	return hash;
}
