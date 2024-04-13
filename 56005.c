static void rt_set_nexthop(struct rtable *rt, __be32 daddr,
			   const struct fib_result *res,
			   struct fib_nh_exception *fnhe,
			   struct fib_info *fi, u16 type, u32 itag,
			   const bool do_cache)
{
	bool cached = false;

	if (fi) {
		struct fib_nh *nh = &FIB_RES_NH(*res);

		if (nh->nh_gw && nh->nh_scope == RT_SCOPE_LINK) {
			rt->rt_gateway = nh->nh_gw;
			rt->rt_uses_gateway = 1;
		}
		dst_init_metrics(&rt->dst, fi->fib_metrics->metrics, true);
		if (fi->fib_metrics != &dst_default_metrics) {
			rt->dst._metrics |= DST_METRICS_REFCOUNTED;
			atomic_inc(&fi->fib_metrics->refcnt);
		}
#ifdef CONFIG_IP_ROUTE_CLASSID
		rt->dst.tclassid = nh->nh_tclassid;
#endif
		rt->dst.lwtstate = lwtstate_get(nh->nh_lwtstate);
		if (unlikely(fnhe))
			cached = rt_bind_exception(rt, fnhe, daddr, do_cache);
		else if (do_cache)
			cached = rt_cache_route(nh, rt);
		if (unlikely(!cached)) {
			/* Routes we intend to cache in nexthop exception or
			 * FIB nexthop have the DST_NOCACHE bit clear.
			 * However, if we are unsuccessful at storing this
			 * route into the cache we really need to set it.
			 */
			if (!rt->rt_gateway)
				rt->rt_gateway = daddr;
			rt_add_uncached_list(rt);
		}
	} else
		rt_add_uncached_list(rt);

#ifdef CONFIG_IP_ROUTE_CLASSID
#ifdef CONFIG_IP_MULTIPLE_TABLES
	set_class_tag(rt, res->tclassid);
#endif
	set_class_tag(rt, itag);
#endif
}
