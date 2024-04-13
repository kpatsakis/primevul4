static __net_exit void proto_exit_net(struct net *net)
{
	remove_proc_entry("protocols", net->proc_net);
}
