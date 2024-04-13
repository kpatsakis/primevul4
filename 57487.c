static unsigned int ip6_dst_mtu_forward(const struct dst_entry *dst)
{
	unsigned int mtu;
	struct inet6_dev *idev;

	if (dst_metric_locked(dst, RTAX_MTU)) {
		mtu = dst_metric_raw(dst, RTAX_MTU);
		if (mtu)
			return mtu;
	}

	mtu = IPV6_MIN_MTU;
	rcu_read_lock();
	idev = __in6_dev_get(dst->dev);
	if (idev)
		mtu = idev->cnf.mtu6;
	rcu_read_unlock();

	return mtu;
}
