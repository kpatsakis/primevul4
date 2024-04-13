void __net_exit ip_vs_control_net_cleanup(struct net *net)
{
	struct netns_ipvs *ipvs = net_ipvs(net);

	ip_vs_trash_cleanup(net);
	ip_vs_stop_estimator(net, &ipvs->tot_stats);
	ip_vs_control_net_cleanup_sysctl(net);
	proc_net_remove(net, "ip_vs_stats_percpu");
	proc_net_remove(net, "ip_vs_stats");
	proc_net_remove(net, "ip_vs");
	free_percpu(ipvs->tot_stats.cpustats);
}
