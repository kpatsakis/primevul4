static struct sock *__udp4_lib_demux_lookup(struct net *net,
					    __be16 loc_port, __be32 loc_addr,
					    __be16 rmt_port, __be32 rmt_addr,
					    int dif)
{
	struct sock *sk, *result;
	struct hlist_nulls_node *node;
	unsigned short hnum = ntohs(loc_port);
	unsigned int hash2 = udp4_portaddr_hash(net, loc_addr, hnum);
	unsigned int slot2 = hash2 & udp_table.mask;
	struct udp_hslot *hslot2 = &udp_table.hash2[slot2];
	INET_ADDR_COOKIE(acookie, rmt_addr, loc_addr)
	const __portpair ports = INET_COMBINED_PORTS(rmt_port, hnum);

	rcu_read_lock();
	result = NULL;
	udp_portaddr_for_each_entry_rcu(sk, node, &hslot2->head) {
		if (INET_MATCH(sk, net, acookie,
			       rmt_addr, loc_addr, ports, dif))
			result = sk;
		/* Only check first socket in chain */
		break;
	}

	if (result) {
		if (unlikely(!atomic_inc_not_zero_hint(&result->sk_refcnt, 2)))
			result = NULL;
		else if (unlikely(!INET_MATCH(sk, net, acookie,
					      rmt_addr, loc_addr,
					      ports, dif))) {
			sock_put(result);
			result = NULL;
		}
	}
	rcu_read_unlock();
	return result;
}
