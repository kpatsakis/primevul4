static unsigned int ipv4_mtu(const struct dst_entry *dst)
{
	const struct rtable *rt = (const struct rtable *) dst;
	unsigned int mtu = rt->rt_pmtu;

	if (!mtu || time_after_eq(jiffies, rt->dst.expires))
		mtu = dst_metric_raw(dst, RTAX_MTU);

	if (mtu)
		return mtu;

	mtu = READ_ONCE(dst->dev->mtu);

	if (unlikely(dst_metric_locked(dst, RTAX_MTU))) {
		if (rt->rt_uses_gateway && mtu > 576)
			mtu = 576;
	}

	mtu = min_t(unsigned int, mtu, IP_MAX_MTU);

	return mtu - lwtunnel_headroom(dst->lwtstate, mtu);
}
