static bool icmpv6_xrlim_allow(struct sock *sk, u8 type,
			       struct flowi6 *fl6)
{
	struct net *net = sock_net(sk);
	struct dst_entry *dst;
	bool res = false;

	/* Informational messages are not limited. */
	if (type & ICMPV6_INFOMSG_MASK)
		return true;

	/* Do not limit pmtu discovery, it would break it. */
	if (type == ICMPV6_PKT_TOOBIG)
		return true;

	/*
	 * Look up the output route.
	 * XXX: perhaps the expire for routing entries cloned by
	 * this lookup should be more aggressive (not longer than timeout).
	 */
	dst = ip6_route_output(net, sk, fl6);
	if (dst->error) {
		IP6_INC_STATS(net, ip6_dst_idev(dst),
			      IPSTATS_MIB_OUTNOROUTES);
	} else if (dst->dev && (dst->dev->flags&IFF_LOOPBACK)) {
		res = true;
	} else {
		struct rt6_info *rt = (struct rt6_info *)dst;
		int tmo = net->ipv6.sysctl.icmpv6_time;

		/* Give more bandwidth to wider prefixes. */
		if (rt->rt6i_dst.plen < 128)
			tmo >>= ((128 - rt->rt6i_dst.plen)>>5);

		if (icmp_global_allow()) {
			struct inet_peer *peer;

			peer = inet_getpeer_v6(net->ipv6.peers,
					       &fl6->daddr, 1);
			res = inet_peer_xrlim_allow(peer, tmo);
			if (peer)
				inet_putpeer(peer);
		}
	}
	dst_release(dst);
	return res;
}
