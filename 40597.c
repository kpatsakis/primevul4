static int __net_init pfkey_net_init(struct net *net)
{
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);
	int rv;

	INIT_HLIST_HEAD(&net_pfkey->table);
	atomic_set(&net_pfkey->socks_nr, 0);

	rv = pfkey_init_proc(net);

	return rv;
}
