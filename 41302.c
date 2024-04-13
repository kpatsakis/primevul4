static int tg3_change_mtu(struct net_device *dev, int new_mtu)
{
	struct tg3 *tp = netdev_priv(dev);
	int err, reset_phy = 0;

	if (new_mtu < TG3_MIN_MTU || new_mtu > TG3_MAX_MTU(tp))
		return -EINVAL;

	if (!netif_running(dev)) {
		/* We'll just catch it later when the
		 * device is up'd.
		 */
		tg3_set_mtu(dev, tp, new_mtu);
		return 0;
	}

	tg3_phy_stop(tp);

	tg3_netif_stop(tp);

	tg3_full_lock(tp, 1);

	tg3_halt(tp, RESET_KIND_SHUTDOWN, 1);

	tg3_set_mtu(dev, tp, new_mtu);

	/* Reset PHY, otherwise the read DMA engine will be in a mode that
	 * breaks all requests to 256 bytes.
	 */
	if (tg3_asic_rev(tp) == ASIC_REV_57766)
		reset_phy = 1;

	err = tg3_restart_hw(tp, reset_phy);

	if (!err)
		tg3_netif_start(tp);

	tg3_full_unlock(tp);

	if (!err)
		tg3_phy_start(tp);

	return err;
}
