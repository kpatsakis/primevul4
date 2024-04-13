int tipc_sk_rht_init(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);

	return rhashtable_init(&tn->sk_rht, &tsk_rht_params);
}
