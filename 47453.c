void ndisc_cleanup(void)
{
#ifdef CONFIG_SYSCTL
	neigh_sysctl_unregister(&nd_tbl.parms);
#endif
	neigh_table_clear(NEIGH_ND_TABLE, &nd_tbl);
	unregister_pernet_subsys(&ndisc_net_ops);
}
