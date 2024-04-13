int __net_init ip_vs_control_net_init(struct net *net)
{
	int idx;
	struct netns_ipvs *ipvs = net_ipvs(net);

	rwlock_init(&ipvs->rs_lock);

	/* Initialize rs_table */
	for (idx = 0; idx < IP_VS_RTAB_SIZE; idx++)
		INIT_LIST_HEAD(&ipvs->rs_table[idx]);

	INIT_LIST_HEAD(&ipvs->dest_trash);
	atomic_set(&ipvs->ftpsvc_counter, 0);
	atomic_set(&ipvs->nullsvc_counter, 0);

	/* procfs stats */
	ipvs->tot_stats.cpustats = alloc_percpu(struct ip_vs_cpu_stats);
	if (!ipvs->tot_stats.cpustats)
		return -ENOMEM;

	spin_lock_init(&ipvs->tot_stats.lock);

	proc_net_fops_create(net, "ip_vs", 0, &ip_vs_info_fops);
	proc_net_fops_create(net, "ip_vs_stats", 0, &ip_vs_stats_fops);
	proc_net_fops_create(net, "ip_vs_stats_percpu", 0,
			     &ip_vs_stats_percpu_fops);

	if (ip_vs_control_net_init_sysctl(net))
		goto err;

	return 0;

err:
	free_percpu(ipvs->tot_stats.cpustats);
	return -ENOMEM;
}
