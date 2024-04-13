void rtnl_set_sk_err(struct net *net, u32 group, int error)
{
	struct sock *rtnl = net->rtnl;

	netlink_set_err(rtnl, 0, group, error);
}
