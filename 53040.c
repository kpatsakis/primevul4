static int atl2_check_link(struct atl2_adapter *adapter)
{
	struct atl2_hw *hw = &adapter->hw;
	struct net_device *netdev = adapter->netdev;
	int ret_val;
	u16 speed, duplex, phy_data;
	int reconfig = 0;

	/* MII_BMSR must read twise */
	atl2_read_phy_reg(hw, MII_BMSR, &phy_data);
	atl2_read_phy_reg(hw, MII_BMSR, &phy_data);
	if (!(phy_data&BMSR_LSTATUS)) { /* link down */
		if (netif_carrier_ok(netdev)) { /* old link state: Up */
			u32 value;
			/* disable rx */
			value = ATL2_READ_REG(hw, REG_MAC_CTRL);
			value &= ~MAC_CTRL_RX_EN;
			ATL2_WRITE_REG(hw, REG_MAC_CTRL, value);
			adapter->link_speed = SPEED_0;
			netif_carrier_off(netdev);
			netif_stop_queue(netdev);
		}
		return 0;
	}

	/* Link Up */
	ret_val = atl2_get_speed_and_duplex(hw, &speed, &duplex);
	if (ret_val)
		return ret_val;
	switch (hw->MediaType) {
	case MEDIA_TYPE_100M_FULL:
		if (speed  != SPEED_100 || duplex != FULL_DUPLEX)
			reconfig = 1;
		break;
	case MEDIA_TYPE_100M_HALF:
		if (speed  != SPEED_100 || duplex != HALF_DUPLEX)
			reconfig = 1;
		break;
	case MEDIA_TYPE_10M_FULL:
		if (speed != SPEED_10 || duplex != FULL_DUPLEX)
			reconfig = 1;
		break;
	case MEDIA_TYPE_10M_HALF:
		if (speed  != SPEED_10 || duplex != HALF_DUPLEX)
			reconfig = 1;
		break;
	}
	/* link result is our setting */
	if (reconfig == 0) {
		if (adapter->link_speed != speed ||
			adapter->link_duplex != duplex) {
			adapter->link_speed = speed;
			adapter->link_duplex = duplex;
			atl2_setup_mac_ctrl(adapter);
			printk(KERN_INFO "%s: %s NIC Link is Up<%d Mbps %s>\n",
				atl2_driver_name, netdev->name,
				adapter->link_speed,
				adapter->link_duplex == FULL_DUPLEX ?
					"Full Duplex" : "Half Duplex");
		}

		if (!netif_carrier_ok(netdev)) { /* Link down -> Up */
			netif_carrier_on(netdev);
			netif_wake_queue(netdev);
		}
		return 0;
	}

	/* change original link status */
	if (netif_carrier_ok(netdev)) {
		u32 value;
		/* disable rx */
		value = ATL2_READ_REG(hw, REG_MAC_CTRL);
		value &= ~MAC_CTRL_RX_EN;
		ATL2_WRITE_REG(hw, REG_MAC_CTRL, value);

		adapter->link_speed = SPEED_0;
		netif_carrier_off(netdev);
		netif_stop_queue(netdev);
	}

	/* auto-neg, insert timer to re-config phy
	 * (if interval smaller than 5 seconds, something strange) */
	if (!test_bit(__ATL2_DOWN, &adapter->flags)) {
		if (!test_and_set_bit(0, &adapter->cfg_phy))
			mod_timer(&adapter->phy_config_timer,
				  round_jiffies(jiffies + 5 * HZ));
	}

	return 0;
}
