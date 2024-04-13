static struct sock *__udp4_lib_mcast_demux_lookup(struct net *net,
						  __be16 loc_port, __be32 loc_addr,
						  __be16 rmt_port, __be32 rmt_addr,
						  int dif)
{
	struct sock *sk, *result;
	struct hlist_nulls_node *node;
	unsigned short hnum = ntohs(loc_port);
	unsigned int count, slot = udp_hashfn(net, hnum, udp_table.mask);
	struct udp_hslot *hslot = &udp_table.hash[slot];

	rcu_read_lock();
begin:
	count = 0;
	result = NULL;
	sk_nulls_for_each_rcu(sk, node, &hslot->head) {
		if (__udp_is_mcast_sock(net, sk,
					loc_port, loc_addr,
					rmt_port, rmt_addr,
					dif, hnum)) {
			result = sk;
			++count;
		}
	}
	/*
	 * if the nulls value we got at the end of this lookup is
	 * not the expected one, we must restart lookup.
	 * We probably met an item that was moved to another chain.
	 */
	if (get_nulls_value(node) != slot)
		goto begin;

	if (result) {
		if (count != 1 ||
		    unlikely(!atomic_inc_not_zero_hint(&result->sk_refcnt, 2)))
			result = NULL;
		else if (unlikely(!__udp_is_mcast_sock(net, result,
						       loc_port, loc_addr,
						       rmt_port, rmt_addr,
						       dif, hnum))) {
			sock_put(result);
			result = NULL;
		}
	}
	rcu_read_unlock();
	return result;
}
