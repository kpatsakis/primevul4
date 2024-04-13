static void __net_exit pfkey_net_exit(struct net *net)
{
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);

	pfkey_exit_proc(net);
	BUG_ON(!hlist_empty(&net_pfkey->table));
}
