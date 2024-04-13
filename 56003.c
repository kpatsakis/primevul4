void rt_flush_dev(struct net_device *dev)
{
	struct net *net = dev_net(dev);
	struct rtable *rt;
	int cpu;

	for_each_possible_cpu(cpu) {
		struct uncached_list *ul = &per_cpu(rt_uncached_list, cpu);

		spin_lock_bh(&ul->lock);
		list_for_each_entry(rt, &ul->head, rt_uncached) {
			if (rt->dst.dev != dev)
				continue;
			rt->dst.dev = net->loopback_dev;
			dev_hold(rt->dst.dev);
			dev_put(dev);
		}
		spin_unlock_bh(&ul->lock);
	}
}
