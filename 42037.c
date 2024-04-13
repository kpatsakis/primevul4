static int __init sock_diag_init(void)
{
	return register_pernet_subsys(&diag_net_ops);
}
