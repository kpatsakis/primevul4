static int generic_kmemdup_compat_sysctl_table(struct nf_proto_net *pn,
					       struct nf_generic_net *gn)
{
#ifdef CONFIG_SYSCTL
#ifdef CONFIG_NF_CONNTRACK_PROC_COMPAT
	pn->ctl_compat_table = kmemdup(generic_compat_sysctl_table,
				       sizeof(generic_compat_sysctl_table),
				       GFP_KERNEL);
	if (!pn->ctl_compat_table)
		return -ENOMEM;

	pn->ctl_compat_table[0].data = &gn->timeout;
#endif
#endif
	return 0;
}
