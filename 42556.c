void br_netfilter_fini(void)
{
	nf_unregister_hooks(br_nf_ops, ARRAY_SIZE(br_nf_ops));
#ifdef CONFIG_SYSCTL
	unregister_sysctl_table(brnf_sysctl_header);
#endif
	dst_entries_destroy(&fake_dst_ops);
}
