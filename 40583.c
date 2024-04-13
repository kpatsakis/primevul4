static void __net_exit pfkey_exit_proc(struct net *net)
{
	remove_proc_entry("pfkey", net->proc_net);
}
