static bool rt_cache_route(struct fib_nh *nh, struct rtable *rt)
{
	struct rtable *orig, *prev, **p;
	bool ret = true;

	if (rt_is_input_route(rt)) {
		p = (struct rtable **)&nh->nh_rth_input;
	} else {
		p = (struct rtable **)raw_cpu_ptr(nh->nh_pcpu_rth_output);
	}
	orig = *p;

	/* hold dst before doing cmpxchg() to avoid race condition
	 * on this dst
	 */
	dst_hold(&rt->dst);
	prev = cmpxchg(p, orig, rt);
	if (prev == orig) {
		if (orig) {
			dst_dev_put(&orig->dst);
			dst_release(&orig->dst);
		}
	} else {
		dst_release(&rt->dst);
		ret = false;
	}

	return ret;
}
