void __net_exit ip_vs_control_net_cleanup_sysctl(struct net *net)
{
	struct netns_ipvs *ipvs = net_ipvs(net);

	cancel_delayed_work_sync(&ipvs->defense_work);
	cancel_work_sync(&ipvs->defense_work.work);
	unregister_net_sysctl_table(ipvs->sysctl_hdr);
}
