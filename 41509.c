static void tg3_set_msglevel(struct net_device *dev, u32 value)
{
	struct tg3 *tp = netdev_priv(dev);
	tp->msg_enable = value;
}
