static int tg3_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct tg3 *tp = netdev_priv(dev);
	struct device *dp = &tp->pdev->dev;

	if (wol->wolopts & ~WAKE_MAGIC)
		return -EINVAL;
	if ((wol->wolopts & WAKE_MAGIC) &&
	    !(tg3_flag(tp, WOL_CAP) && device_can_wakeup(dp)))
		return -EINVAL;

	device_set_wakeup_enable(dp, wol->wolopts & WAKE_MAGIC);

	spin_lock_bh(&tp->lock);
	if (device_may_wakeup(dp))
		tg3_flag_set(tp, WOL_ENABLE);
	else
		tg3_flag_clear(tp, WOL_ENABLE);
	spin_unlock_bh(&tp->lock);

	return 0;
}
