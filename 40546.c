	__acquires(rcu)
{
	struct net *net = seq_file_net(f);
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);

	rcu_read_lock();
	return seq_hlist_start_head_rcu(&net_pfkey->table, *ppos);
}
