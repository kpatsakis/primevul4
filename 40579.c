static int pfkey_dump_sa(struct pfkey_sock *pfk)
{
	struct net *net = sock_net(&pfk->sk);
	return xfrm_state_walk(net, &pfk->dump.u.state, dump_sa, (void *) pfk);
}
