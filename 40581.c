static int pfkey_dump_sp(struct pfkey_sock *pfk)
{
	struct net *net = sock_net(&pfk->sk);
	return xfrm_policy_walk(net, &pfk->dump.u.policy, dump_sp, (void *) pfk);
}
