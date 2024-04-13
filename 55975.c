int fib_multipath_hash(const struct fib_info *fi, const struct flowi4 *fl4,
		       const struct sk_buff *skb)
{
	struct net *net = fi->fib_net;
	struct flow_keys hash_keys;
	u32 mhash;

	switch (net->ipv4.sysctl_fib_multipath_hash_policy) {
	case 0:
		memset(&hash_keys, 0, sizeof(hash_keys));
		hash_keys.control.addr_type = FLOW_DISSECTOR_KEY_IPV4_ADDRS;
		if (skb) {
			ip_multipath_l3_keys(skb, &hash_keys);
		} else {
			hash_keys.addrs.v4addrs.src = fl4->saddr;
			hash_keys.addrs.v4addrs.dst = fl4->daddr;
		}
		break;
	case 1:
		/* skb is currently provided only when forwarding */
		if (skb) {
			unsigned int flag = FLOW_DISSECTOR_F_STOP_AT_ENCAP;
			struct flow_keys keys;

			/* short-circuit if we already have L4 hash present */
			if (skb->l4_hash)
				return skb_get_hash_raw(skb) >> 1;
			memset(&hash_keys, 0, sizeof(hash_keys));
			skb_flow_dissect_flow_keys(skb, &keys, flag);
			hash_keys.addrs.v4addrs.src = keys.addrs.v4addrs.src;
			hash_keys.addrs.v4addrs.dst = keys.addrs.v4addrs.dst;
			hash_keys.ports.src = keys.ports.src;
			hash_keys.ports.dst = keys.ports.dst;
			hash_keys.basic.ip_proto = keys.basic.ip_proto;
		} else {
			memset(&hash_keys, 0, sizeof(hash_keys));
			hash_keys.control.addr_type = FLOW_DISSECTOR_KEY_IPV4_ADDRS;
			hash_keys.addrs.v4addrs.src = fl4->saddr;
			hash_keys.addrs.v4addrs.dst = fl4->daddr;
			hash_keys.ports.src = fl4->fl4_sport;
			hash_keys.ports.dst = fl4->fl4_dport;
			hash_keys.basic.ip_proto = fl4->flowi4_proto;
		}
		break;
	}
	mhash = flow_hash_from_keys(&hash_keys);

	return mhash >> 1;
}
