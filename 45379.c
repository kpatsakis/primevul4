static inline struct pppoe_net *pppoe_pernet(struct net *net)
{
	BUG_ON(!net);

	return net_generic(net, pppoe_net_id);
}
