static int __net_init sctp_proc_init(struct net *net)
{
#ifdef CONFIG_PROC_FS
	net->sctp.proc_net_sctp = proc_net_mkdir(net, "sctp", net->proc_net);
	if (!net->sctp.proc_net_sctp)
		goto out_proc_net_sctp;
	if (sctp_snmp_proc_init(net))
		goto out_snmp_proc_init;
	if (sctp_eps_proc_init(net))
		goto out_eps_proc_init;
	if (sctp_assocs_proc_init(net))
		goto out_assocs_proc_init;
	if (sctp_remaddr_proc_init(net))
		goto out_remaddr_proc_init;

	return 0;

out_remaddr_proc_init:
	sctp_assocs_proc_exit(net);
out_assocs_proc_init:
	sctp_eps_proc_exit(net);
out_eps_proc_init:
	sctp_snmp_proc_exit(net);
out_snmp_proc_init:
	remove_proc_entry("sctp", net->proc_net);
	net->sctp.proc_net_sctp = NULL;
out_proc_net_sctp:
	return -ENOMEM;
#endif /* CONFIG_PROC_FS */
	return 0;
}
