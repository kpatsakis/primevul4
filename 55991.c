static void ipv4_confirm_neigh(const struct dst_entry *dst, const void *daddr)
{
	struct net_device *dev = dst->dev;
	const __be32 *pkey = daddr;
	const struct rtable *rt;

	rt = (const struct rtable *)dst;
	if (rt->rt_gateway)
		pkey = (const __be32 *)&rt->rt_gateway;
	else if (!daddr ||
		 (rt->rt_flags &
		  (RTCF_MULTICAST | RTCF_BROADCAST | RTCF_LOCAL)))
		return;

	__ipv4_confirm_neigh(dev, *(__force u32 *)pkey);
}
