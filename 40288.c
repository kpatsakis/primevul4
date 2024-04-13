static int rtnl_bridge_notify(struct net_device *dev, u16 flags)
{
	struct net *net = dev_net(dev);
	struct net_device *br_dev = netdev_master_upper_dev_get(dev);
	struct sk_buff *skb;
	int err = -EOPNOTSUPP;

	skb = nlmsg_new(bridge_nlmsg_size(), GFP_ATOMIC);
	if (!skb) {
		err = -ENOMEM;
		goto errout;
	}

	if ((!flags || (flags & BRIDGE_FLAGS_MASTER)) &&
	    br_dev && br_dev->netdev_ops->ndo_bridge_getlink) {
		err = br_dev->netdev_ops->ndo_bridge_getlink(skb, 0, 0, dev, 0);
		if (err < 0)
			goto errout;
	}

	if ((flags & BRIDGE_FLAGS_SELF) &&
	    dev->netdev_ops->ndo_bridge_getlink) {
		err = dev->netdev_ops->ndo_bridge_getlink(skb, 0, 0, dev, 0);
		if (err < 0)
			goto errout;
	}

	rtnl_notify(skb, net, 0, RTNLGRP_LINK, NULL, GFP_ATOMIC);
	return 0;
errout:
	WARN_ON(err == -EMSGSIZE);
	kfree_skb(skb);
	rtnl_set_sk_err(net, RTNLGRP_LINK, err);
	return err;
}
