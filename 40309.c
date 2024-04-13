struct net *rtnl_link_get_net(struct net *src_net, struct nlattr *tb[])
{
	struct net *net;
	/* Examine the link attributes and figure out which
	 * network namespace we are talking about.
	 */
	if (tb[IFLA_NET_NS_PID])
		net = get_net_ns_by_pid(nla_get_u32(tb[IFLA_NET_NS_PID]));
	else if (tb[IFLA_NET_NS_FD])
		net = get_net_ns_by_fd(nla_get_u32(tb[IFLA_NET_NS_FD]));
	else
		net = get_net(src_net);
	return net;
}
