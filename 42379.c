static bool __ip_vs_addr_is_local_v6(struct net *net,
				     const struct in6_addr *addr)
{
	struct flowi6 fl6 = {
		.daddr = *addr,
	};
	struct dst_entry *dst = ip6_route_output(net, NULL, &fl6);
	bool is_local;

	is_local = !dst->error && dst->dev && (dst->dev->flags & IFF_LOOPBACK);

	dst_release(dst);
	return is_local;
}
