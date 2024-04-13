int __init br_netfilter_init(void)
{
	int ret;

	ret = dst_entries_init(&fake_dst_ops);
	if (ret < 0)
		return ret;

	ret = nf_register_hooks(br_nf_ops, ARRAY_SIZE(br_nf_ops));
	if (ret < 0) {
		dst_entries_destroy(&fake_dst_ops);
		return ret;
	}
#ifdef CONFIG_SYSCTL
	brnf_sysctl_header = register_sysctl_paths(brnf_path, brnf_table);
	if (brnf_sysctl_header == NULL) {
		printk(KERN_WARNING
		       "br_netfilter: can't register to sysctl.\n");
		nf_unregister_hooks(br_nf_ops, ARRAY_SIZE(br_nf_ops));
		dst_entries_destroy(&fake_dst_ops);
		return -ENOMEM;
	}
#endif
	printk(KERN_NOTICE "Bridge firewalling registered\n");
	return 0;
}
