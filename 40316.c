static int rtnl_port_fill(struct sk_buff *skb, struct net_device *dev)
{
	int err;

	if (!dev->netdev_ops->ndo_get_vf_port || !dev->dev.parent)
		return 0;

	err = rtnl_port_self_fill(skb, dev);
	if (err)
		return err;

	if (dev_num_vf(dev->dev.parent)) {
		err = rtnl_vf_ports_fill(skb, dev);
		if (err)
			return err;
	}

	return 0;
}
