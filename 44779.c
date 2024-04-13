static void __exit nf_nat_irc_fini(void)
{
	RCU_INIT_POINTER(nf_nat_irc_hook, NULL);
	synchronize_rcu();
}
