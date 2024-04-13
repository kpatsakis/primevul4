int __net_init ip_vs_control_net_init_sysctl(struct net *net)
{
	int idx;
	struct netns_ipvs *ipvs = net_ipvs(net);
	struct ctl_table *tbl;

	atomic_set(&ipvs->dropentry, 0);
	spin_lock_init(&ipvs->dropentry_lock);
	spin_lock_init(&ipvs->droppacket_lock);
	spin_lock_init(&ipvs->securetcp_lock);

	if (!net_eq(net, &init_net)) {
		tbl = kmemdup(vs_vars, sizeof(vs_vars), GFP_KERNEL);
		if (tbl == NULL)
			return -ENOMEM;
	} else
		tbl = vs_vars;
	/* Initialize sysctl defaults */
	idx = 0;
	ipvs->sysctl_amemthresh = 1024;
	tbl[idx++].data = &ipvs->sysctl_amemthresh;
	ipvs->sysctl_am_droprate = 10;
	tbl[idx++].data = &ipvs->sysctl_am_droprate;
	tbl[idx++].data = &ipvs->sysctl_drop_entry;
	tbl[idx++].data = &ipvs->sysctl_drop_packet;
#ifdef CONFIG_IP_VS_NFCT
	tbl[idx++].data = &ipvs->sysctl_conntrack;
#endif
	tbl[idx++].data = &ipvs->sysctl_secure_tcp;
	ipvs->sysctl_snat_reroute = 1;
	tbl[idx++].data = &ipvs->sysctl_snat_reroute;
	ipvs->sysctl_sync_ver = 1;
	tbl[idx++].data = &ipvs->sysctl_sync_ver;
	ipvs->sysctl_sync_ports = 1;
	tbl[idx++].data = &ipvs->sysctl_sync_ports;
	ipvs->sysctl_sync_qlen_max = nr_free_buffer_pages() / 32;
	tbl[idx++].data = &ipvs->sysctl_sync_qlen_max;
	ipvs->sysctl_sync_sock_size = 0;
	tbl[idx++].data = &ipvs->sysctl_sync_sock_size;
	tbl[idx++].data = &ipvs->sysctl_cache_bypass;
	tbl[idx++].data = &ipvs->sysctl_expire_nodest_conn;
	tbl[idx++].data = &ipvs->sysctl_expire_quiescent_template;
	ipvs->sysctl_sync_threshold[0] = DEFAULT_SYNC_THRESHOLD;
	ipvs->sysctl_sync_threshold[1] = DEFAULT_SYNC_PERIOD;
	tbl[idx].data = &ipvs->sysctl_sync_threshold;
	tbl[idx++].maxlen = sizeof(ipvs->sysctl_sync_threshold);
	ipvs->sysctl_sync_refresh_period = DEFAULT_SYNC_REFRESH_PERIOD;
	tbl[idx++].data = &ipvs->sysctl_sync_refresh_period;
	ipvs->sysctl_sync_retries = clamp_t(int, DEFAULT_SYNC_RETRIES, 0, 3);
	tbl[idx++].data = &ipvs->sysctl_sync_retries;
	tbl[idx++].data = &ipvs->sysctl_nat_icmp_send;


	ipvs->sysctl_hdr = register_net_sysctl(net, "net/ipv4/vs", tbl);
	if (ipvs->sysctl_hdr == NULL) {
		if (!net_eq(net, &init_net))
			kfree(tbl);
		return -ENOMEM;
	}
	ip_vs_start_estimator(net, &ipvs->tot_stats);
	ipvs->sysctl_tbl = tbl;
	/* Schedule defense work */
	INIT_DELAYED_WORK(&ipvs->defense_work, defense_work_handler);
	schedule_delayed_work(&ipvs->defense_work, DEFENSE_TIMER_PERIOD);

	return 0;
}
