int nfnetlink_has_listeners(struct net *net, unsigned int group)
{
	return netlink_has_listeners(net->nfnl, group);
}
