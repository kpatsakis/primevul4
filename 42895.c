static void ipip6_tunnel_setup(struct net_device *dev)
{
	dev->netdev_ops		= &ipip6_netdev_ops;
	dev->destructor 	= ipip6_dev_free;

	dev->type		= ARPHRD_SIT;
	dev->hard_header_len 	= LL_MAX_HEADER + sizeof(struct iphdr);
	dev->mtu		= ETH_DATA_LEN - sizeof(struct iphdr);
	dev->flags		= IFF_NOARP;
	dev->priv_flags	       &= ~IFF_XMIT_DST_RELEASE;
	dev->iflink		= 0;
	dev->addr_len		= 4;
	dev->features		|= NETIF_F_NETNS_LOCAL;
	dev->features		|= NETIF_F_LLTX;
}
