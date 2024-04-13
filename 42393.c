int __init ip_vs_control_init(void)
{
	int idx;
	int ret;

	EnterFunction(2);

	/* Initialize svc_table, ip_vs_svc_fwm_table, rs_table */
	for (idx = 0; idx < IP_VS_SVC_TAB_SIZE; idx++) {
		INIT_LIST_HEAD(&ip_vs_svc_table[idx]);
		INIT_LIST_HEAD(&ip_vs_svc_fwm_table[idx]);
	}

	smp_wmb();	/* Do we really need it now ? */

	ret = register_netdevice_notifier(&ip_vs_dst_notifier);
	if (ret < 0)
		return ret;

	LeaveFunction(2);
	return 0;
}
