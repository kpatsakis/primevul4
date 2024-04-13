static void pfkey_insert(struct sock *sk)
{
	struct net *net = sock_net(sk);
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);

	mutex_lock(&pfkey_mutex);
	sk_add_node_rcu(sk, &net_pfkey->table);
	mutex_unlock(&pfkey_mutex);
}
