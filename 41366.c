static void tg3_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct tg3 *tp = netdev_priv(dev);

	if (tg3_flag(tp, WOL_CAP) && device_can_wakeup(&tp->pdev->dev))
		wol->supported = WAKE_MAGIC;
	else
		wol->supported = 0;
	wol->wolopts = 0;
	if (tg3_flag(tp, WOL_ENABLE) && device_can_wakeup(&tp->pdev->dev))
		wol->wolopts = WAKE_MAGIC;
	memset(&wol->sopass, 0, sizeof(wol->sopass));
}
