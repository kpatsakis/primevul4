static int tg3_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct mii_ioctl_data *data = if_mii(ifr);
	struct tg3 *tp = netdev_priv(dev);
	int err;

	if (tg3_flag(tp, USE_PHYLIB)) {
		struct phy_device *phydev;
		if (!(tp->phy_flags & TG3_PHYFLG_IS_CONNECTED))
			return -EAGAIN;
		phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];
		return phy_mii_ioctl(phydev, ifr, cmd);
	}

	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = tp->phy_addr;

		/* fallthru */
	case SIOCGMIIREG: {
		u32 mii_regval;

		if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES)
			break;			/* We have no PHY */

		if (!netif_running(dev))
			return -EAGAIN;

		spin_lock_bh(&tp->lock);
		err = __tg3_readphy(tp, data->phy_id & 0x1f,
				    data->reg_num & 0x1f, &mii_regval);
		spin_unlock_bh(&tp->lock);

		data->val_out = mii_regval;

		return err;
	}

	case SIOCSMIIREG:
		if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES)
			break;			/* We have no PHY */

		if (!netif_running(dev))
			return -EAGAIN;

		spin_lock_bh(&tp->lock);
		err = __tg3_writephy(tp, data->phy_id & 0x1f,
				     data->reg_num & 0x1f, data->val_in);
		spin_unlock_bh(&tp->lock);

		return err;

	case SIOCSHWTSTAMP:
		return tg3_hwtstamp_ioctl(dev, ifr, cmd);

	default:
		/* do nothing */
		break;
	}
	return -EOPNOTSUPP;
}
