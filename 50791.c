struct sk_buff *nfnetlink_alloc_skb(struct net *net, unsigned int size,
				    u32 dst_portid, gfp_t gfp_mask)
{
	return netlink_alloc_skb(net->nfnl, size, dst_portid, gfp_mask);
}
