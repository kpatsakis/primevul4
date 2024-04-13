struct net_device *rtnl_create_link(struct net *net,
	char *ifname, const struct rtnl_link_ops *ops, struct nlattr *tb[])
{
	int err;
	struct net_device *dev;
	unsigned int num_tx_queues = 1;
	unsigned int num_rx_queues = 1;

	if (tb[IFLA_NUM_TX_QUEUES])
		num_tx_queues = nla_get_u32(tb[IFLA_NUM_TX_QUEUES]);
	else if (ops->get_num_tx_queues)
		num_tx_queues = ops->get_num_tx_queues();

	if (tb[IFLA_NUM_RX_QUEUES])
		num_rx_queues = nla_get_u32(tb[IFLA_NUM_RX_QUEUES]);
	else if (ops->get_num_rx_queues)
		num_rx_queues = ops->get_num_rx_queues();

	err = -ENOMEM;
	dev = alloc_netdev_mqs(ops->priv_size, ifname, ops->setup,
			       num_tx_queues, num_rx_queues);
	if (!dev)
		goto err;

	dev_net_set(dev, net);
	dev->rtnl_link_ops = ops;
	dev->rtnl_link_state = RTNL_LINK_INITIALIZING;

	if (tb[IFLA_MTU])
		dev->mtu = nla_get_u32(tb[IFLA_MTU]);
	if (tb[IFLA_ADDRESS]) {
		memcpy(dev->dev_addr, nla_data(tb[IFLA_ADDRESS]),
				nla_len(tb[IFLA_ADDRESS]));
		dev->addr_assign_type = NET_ADDR_SET;
	}
	if (tb[IFLA_BROADCAST])
		memcpy(dev->broadcast, nla_data(tb[IFLA_BROADCAST]),
				nla_len(tb[IFLA_BROADCAST]));
	if (tb[IFLA_TXQLEN])
		dev->tx_queue_len = nla_get_u32(tb[IFLA_TXQLEN]);
	if (tb[IFLA_OPERSTATE])
		set_operstate(dev, nla_get_u8(tb[IFLA_OPERSTATE]));
	if (tb[IFLA_LINKMODE])
		dev->link_mode = nla_get_u8(tb[IFLA_LINKMODE]);
	if (tb[IFLA_GROUP])
		dev_set_group(dev, nla_get_u32(tb[IFLA_GROUP]));

	return dev;

err:
	return ERR_PTR(err);
}
