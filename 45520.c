static struct ipx_interface *ipxitf_find_using_phys(struct net_device *dev,
						    __be16 datalink)
{
	struct ipx_interface *i;

	spin_lock_bh(&ipx_interfaces_lock);
	i = __ipxitf_find_using_phys(dev, datalink);
	if (i)
		ipxitf_hold(i);
	spin_unlock_bh(&ipx_interfaces_lock);
	return i;
}
