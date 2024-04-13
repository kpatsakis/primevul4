static void *pfkey_seq_next(struct seq_file *f, void *v, loff_t *ppos)
{
	struct net *net = seq_file_net(f);
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);

	return seq_hlist_next_rcu(v, &net_pfkey->table, ppos);
}
