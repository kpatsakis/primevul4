static bool netlink_compare(struct net *net, struct sock *sk)
{
	return net_eq(sock_net(sk), net);
}
