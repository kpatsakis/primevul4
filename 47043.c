static int virtnet_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < MIN_MTU || new_mtu > MAX_MTU)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}
