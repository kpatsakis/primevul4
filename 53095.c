static int atl2_set_settings(struct net_device *netdev,
	struct ethtool_cmd *ecmd)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;

	while (test_and_set_bit(__ATL2_RESETTING, &adapter->flags))
		msleep(1);

	if (ecmd->autoneg == AUTONEG_ENABLE) {
#define MY_ADV_MASK	(ADVERTISE_10_HALF | \
			 ADVERTISE_10_FULL | \
			 ADVERTISE_100_HALF| \
			 ADVERTISE_100_FULL)

		if ((ecmd->advertising & MY_ADV_MASK) == MY_ADV_MASK) {
			hw->MediaType = MEDIA_TYPE_AUTO_SENSOR;
			hw->autoneg_advertised =  MY_ADV_MASK;
		} else if ((ecmd->advertising & MY_ADV_MASK) ==
				ADVERTISE_100_FULL) {
			hw->MediaType = MEDIA_TYPE_100M_FULL;
			hw->autoneg_advertised = ADVERTISE_100_FULL;
		} else if ((ecmd->advertising & MY_ADV_MASK) ==
				ADVERTISE_100_HALF) {
			hw->MediaType = MEDIA_TYPE_100M_HALF;
			hw->autoneg_advertised = ADVERTISE_100_HALF;
		} else if ((ecmd->advertising & MY_ADV_MASK) ==
				ADVERTISE_10_FULL) {
			hw->MediaType = MEDIA_TYPE_10M_FULL;
			hw->autoneg_advertised = ADVERTISE_10_FULL;
		}  else if ((ecmd->advertising & MY_ADV_MASK) ==
				ADVERTISE_10_HALF) {
			hw->MediaType = MEDIA_TYPE_10M_HALF;
			hw->autoneg_advertised = ADVERTISE_10_HALF;
		} else {
			clear_bit(__ATL2_RESETTING, &adapter->flags);
			return -EINVAL;
		}
		ecmd->advertising = hw->autoneg_advertised |
			ADVERTISED_TP | ADVERTISED_Autoneg;
	} else {
		clear_bit(__ATL2_RESETTING, &adapter->flags);
		return -EINVAL;
	}

	/* reset the link */
	if (netif_running(adapter->netdev)) {
		atl2_down(adapter);
		atl2_up(adapter);
	} else
		atl2_reset_hw(&adapter->hw);

	clear_bit(__ATL2_RESETTING, &adapter->flags);
	return 0;
}
