static void fnhe_flush_routes(struct fib_nh_exception *fnhe)
{
	struct rtable *rt;

	rt = rcu_dereference(fnhe->fnhe_rth_input);
	if (rt) {
		RCU_INIT_POINTER(fnhe->fnhe_rth_input, NULL);
		dst_dev_put(&rt->dst);
		dst_release(&rt->dst);
	}
	rt = rcu_dereference(fnhe->fnhe_rth_output);
	if (rt) {
		RCU_INIT_POINTER(fnhe->fnhe_rth_output, NULL);
		dst_dev_put(&rt->dst);
		dst_release(&rt->dst);
	}
}
