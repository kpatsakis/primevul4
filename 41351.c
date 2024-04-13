static u32 tg3_get_msglevel(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);
	return tp->msg_enable;
}
