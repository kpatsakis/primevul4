static int __udp6_lib_mcast_deliver(struct net *net, struct sk_buff *skb,
		const struct in6_addr *saddr, const struct in6_addr *daddr,
		struct udp_table *udptable, int proto)
{
	struct sock *sk, *stack[256 / sizeof(struct sock *)];
	const struct udphdr *uh = udp_hdr(skb);
	struct hlist_nulls_node *node;
	unsigned short hnum = ntohs(uh->dest);
	struct udp_hslot *hslot = udp_hashslot(udptable, net, hnum);
	int dif = inet6_iif(skb);
	unsigned int count = 0, offset = offsetof(typeof(*sk), sk_nulls_node);
	unsigned int hash2 = 0, hash2_any = 0, use_hash2 = (hslot->count > 10);
	bool inner_flushed = false;

	if (use_hash2) {
		hash2_any = udp6_portaddr_hash(net, &in6addr_any, hnum) &
			    udp_table.mask;
		hash2 = udp6_portaddr_hash(net, daddr, hnum) & udp_table.mask;
start_lookup:
		hslot = &udp_table.hash2[hash2];
		offset = offsetof(typeof(*sk), __sk_common.skc_portaddr_node);
	}

	spin_lock(&hslot->lock);
	sk_nulls_for_each_entry_offset(sk, node, &hslot->head, offset) {
		if (__udp_v6_is_mcast_sock(net, sk,
					   uh->dest, daddr,
					   uh->source, saddr,
					   dif, hnum) &&
		    /* If zero checksum and no_check is not on for
		     * the socket then skip it.
		     */
		    (uh->check || udp_sk(sk)->no_check6_rx)) {
			if (unlikely(count == ARRAY_SIZE(stack))) {
				flush_stack(stack, count, skb, ~0);
				inner_flushed = true;
				count = 0;
			}
			stack[count++] = sk;
			sock_hold(sk);
		}
	}

	spin_unlock(&hslot->lock);

	/* Also lookup *:port if we are using hash2 and haven't done so yet. */
	if (use_hash2 && hash2 != hash2_any) {
		hash2 = hash2_any;
		goto start_lookup;
	}

	if (count) {
		flush_stack(stack, count, skb, count - 1);
	} else {
		if (!inner_flushed)
			UDP_INC_STATS_BH(net, UDP_MIB_IGNOREDMULTI,
					 proto == IPPROTO_UDPLITE);
		consume_skb(skb);
	}
	return 0;
}
