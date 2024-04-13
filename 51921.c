static inline struct ppp_net *ppp_pernet(struct net *net)
{
	BUG_ON(!net);

	return net_generic(net, ppp_net_id);
}
