static inline struct dccp_net *dccp_pernet(struct net *net)
{
	return net_generic(net, dccp_net_id);
}
