static void __exit nf_defrag_fini(void)
{
	nf_unregister_hooks(ipv6_defrag_ops, ARRAY_SIZE(ipv6_defrag_ops));
	nf_ct_frag6_cleanup();
}
