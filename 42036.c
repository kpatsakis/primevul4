static void __exit sock_diag_exit(void)
{
	unregister_pernet_subsys(&diag_net_ops);
}
