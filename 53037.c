static int atl2_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;

	if ((new_mtu < 40) || (new_mtu > (ETH_DATA_LEN + VLAN_SIZE)))
		return -EINVAL;

	/* set MTU */
	if (hw->max_frame_size != new_mtu) {
		netdev->mtu = new_mtu;
		ATL2_WRITE_REG(hw, REG_MTU, new_mtu + ENET_HEADER_SIZE +
			VLAN_SIZE + ETHERNET_FCS_SIZE);
	}

	return 0;
}
