static void __net_exit packet_net_exit(struct net *net)
{
	remove_proc_entry("packet", net->proc_net);
}
