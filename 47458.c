int __init ndisc_init(void)
{
	int err;

	err = register_pernet_subsys(&ndisc_net_ops);
	if (err)
		return err;
	/*
	 * Initialize the neighbour table
	 */
	neigh_table_init(NEIGH_ND_TABLE, &nd_tbl);

#ifdef CONFIG_SYSCTL
	err = neigh_sysctl_register(NULL, &nd_tbl.parms,
				    ndisc_ifinfo_sysctl_change);
	if (err)
		goto out_unregister_pernet;
out:
#endif
	return err;

#ifdef CONFIG_SYSCTL
out_unregister_pernet:
	unregister_pernet_subsys(&ndisc_net_ops);
	goto out;
#endif
}
