void br_netfilter_rtable_init(struct net_bridge *br)
{
	struct rtable *rt = &br->fake_rtable;

	atomic_set(&rt->dst.__refcnt, 1);
	rt->dst.dev = br->dev;
	rt->dst.path = &rt->dst;
	dst_metric_set(&rt->dst, RTAX_MTU, 1500);
	rt->dst.flags	= DST_NOXFRM;
	rt->dst.ops = &fake_dst_ops;
}
