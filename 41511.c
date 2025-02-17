static int tg3_set_pauseparam(struct net_device *dev, struct ethtool_pauseparam *epause)
{
	struct tg3 *tp = netdev_priv(dev);
	int err = 0;

	if (tg3_flag(tp, USE_PHYLIB)) {
		u32 newadv;
		struct phy_device *phydev;

		phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];

		if (!(phydev->supported & SUPPORTED_Pause) ||
		    (!(phydev->supported & SUPPORTED_Asym_Pause) &&
		     (epause->rx_pause != epause->tx_pause)))
			return -EINVAL;

		tp->link_config.flowctrl = 0;
		if (epause->rx_pause) {
			tp->link_config.flowctrl |= FLOW_CTRL_RX;

			if (epause->tx_pause) {
				tp->link_config.flowctrl |= FLOW_CTRL_TX;
				newadv = ADVERTISED_Pause;
			} else
				newadv = ADVERTISED_Pause |
					 ADVERTISED_Asym_Pause;
		} else if (epause->tx_pause) {
			tp->link_config.flowctrl |= FLOW_CTRL_TX;
			newadv = ADVERTISED_Asym_Pause;
		} else
			newadv = 0;

		if (epause->autoneg)
			tg3_flag_set(tp, PAUSE_AUTONEG);
		else
			tg3_flag_clear(tp, PAUSE_AUTONEG);

		if (tp->phy_flags & TG3_PHYFLG_IS_CONNECTED) {
			u32 oldadv = phydev->advertising &
				     (ADVERTISED_Pause | ADVERTISED_Asym_Pause);
			if (oldadv != newadv) {
				phydev->advertising &=
					~(ADVERTISED_Pause |
					  ADVERTISED_Asym_Pause);
				phydev->advertising |= newadv;
				if (phydev->autoneg) {
					/*
					 * Always renegotiate the link to
					 * inform our link partner of our
					 * flow control settings, even if the
					 * flow control is forced.  Let
					 * tg3_adjust_link() do the final
					 * flow control setup.
					 */
					return phy_start_aneg(phydev);
				}
			}

			if (!epause->autoneg)
				tg3_setup_flow_control(tp, 0, 0);
		} else {
			tp->link_config.advertising &=
					~(ADVERTISED_Pause |
					  ADVERTISED_Asym_Pause);
			tp->link_config.advertising |= newadv;
		}
	} else {
		int irq_sync = 0;

		if (netif_running(dev)) {
			tg3_netif_stop(tp);
			irq_sync = 1;
		}

		tg3_full_lock(tp, irq_sync);

		if (epause->autoneg)
			tg3_flag_set(tp, PAUSE_AUTONEG);
		else
			tg3_flag_clear(tp, PAUSE_AUTONEG);
		if (epause->rx_pause)
			tp->link_config.flowctrl |= FLOW_CTRL_RX;
		else
			tp->link_config.flowctrl &= ~FLOW_CTRL_RX;
		if (epause->tx_pause)
			tp->link_config.flowctrl |= FLOW_CTRL_TX;
		else
			tp->link_config.flowctrl &= ~FLOW_CTRL_TX;

		if (netif_running(dev)) {
			tg3_halt(tp, RESET_KIND_SHUTDOWN, 1);
			err = tg3_restart_hw(tp, 1);
			if (!err)
				tg3_netif_start(tp);
		}

		tg3_full_unlock(tp);
	}

	return err;
}
