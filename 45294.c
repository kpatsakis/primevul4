void raw6_proc_exit(void)
{
	unregister_pernet_subsys(&raw6_net_ops);
}
