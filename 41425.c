static int tg3_nway_reset(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);
	int r;

	if (!netif_running(dev))
		return -EAGAIN;

	if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES)
		return -EINVAL;

	if (tg3_flag(tp, USE_PHYLIB)) {
		if (!(tp->phy_flags & TG3_PHYFLG_IS_CONNECTED))
			return -EAGAIN;
		r = phy_start_aneg(tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR]);
	} else {
		u32 bmcr;

		spin_lock_bh(&tp->lock);
		r = -EINVAL;
		tg3_readphy(tp, MII_BMCR, &bmcr);
		if (!tg3_readphy(tp, MII_BMCR, &bmcr) &&
		    ((bmcr & BMCR_ANENABLE) ||
		     (tp->phy_flags & TG3_PHYFLG_PARALLEL_DETECT))) {
			tg3_writephy(tp, MII_BMCR, bmcr | BMCR_ANRESTART |
						   BMCR_ANENABLE);
			r = 0;
		}
		spin_unlock_bh(&tp->lock);
	}

	return r;
}
