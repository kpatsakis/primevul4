static void __ip_rt_update_pmtu(struct rtable *rt, struct flowi4 *fl4, u32 mtu)
{
	struct dst_entry *dst = &rt->dst;
	struct fib_result res;

	if (dst_metric_locked(dst, RTAX_MTU))
		return;

	if (ipv4_mtu(dst) < mtu)
		return;

	if (mtu < ip_rt_min_pmtu)
		mtu = ip_rt_min_pmtu;

	if (rt->rt_pmtu == mtu &&
	    time_before(jiffies, dst->expires - ip_rt_mtu_expires / 2))
		return;

	rcu_read_lock();
	if (fib_lookup(dev_net(dst->dev), fl4, &res, 0) == 0) {
		struct fib_nh *nh = &FIB_RES_NH(res);

		update_or_create_fnhe(nh, fl4->daddr, 0, mtu,
				      jiffies + ip_rt_mtu_expires);
	}
	rcu_read_unlock();
}
