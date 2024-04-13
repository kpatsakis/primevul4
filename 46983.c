static void sctp_proc_exit(struct net *net)
{
#ifdef CONFIG_PROC_FS
	sctp_snmp_proc_exit(net);
	sctp_eps_proc_exit(net);
	sctp_assocs_proc_exit(net);
	sctp_remaddr_proc_exit(net);

	remove_proc_entry("sctp", net->proc_net);
	net->sctp.proc_net_sctp = NULL;
#endif
}
