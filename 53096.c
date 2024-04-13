static void atl2_setup_mac_ctrl(struct atl2_adapter *adapter)
{
	u32 value;
	struct atl2_hw *hw = &adapter->hw;
	struct net_device *netdev = adapter->netdev;

	/* Config MAC CTRL Register */
	value = MAC_CTRL_TX_EN | MAC_CTRL_RX_EN | MAC_CTRL_MACLP_CLK_PHY;

	/* duplex */
	if (FULL_DUPLEX == adapter->link_duplex)
		value |= MAC_CTRL_DUPLX;

	/* flow control */
	value |= (MAC_CTRL_TX_FLOW | MAC_CTRL_RX_FLOW);

	/* PAD & CRC */
	value |= (MAC_CTRL_ADD_CRC | MAC_CTRL_PAD);

	/* preamble length */
	value |= (((u32)adapter->hw.preamble_len & MAC_CTRL_PRMLEN_MASK) <<
		MAC_CTRL_PRMLEN_SHIFT);

	/* vlan */
	__atl2_vlan_mode(netdev->features, &value);

	/* filter mode */
	value |= MAC_CTRL_BC_EN;
	if (netdev->flags & IFF_PROMISC)
		value |= MAC_CTRL_PROMIS_EN;
	else if (netdev->flags & IFF_ALLMULTI)
		value |= MAC_CTRL_MC_ALL_EN;

	/* half retry buffer */
	value |= (((u32)(adapter->hw.retry_buf &
		MAC_CTRL_HALF_LEFT_BUF_MASK)) << MAC_CTRL_HALF_LEFT_BUF_SHIFT);

	ATL2_WRITE_REG(hw, REG_MAC_CTRL, value);
}
