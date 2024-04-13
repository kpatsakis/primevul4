static void atl2_set_multi(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct atl2_hw *hw = &adapter->hw;
	struct netdev_hw_addr *ha;
	u32 rctl;
	u32 hash_value;

	/* Check for Promiscuous and All Multicast modes */
	rctl = ATL2_READ_REG(hw, REG_MAC_CTRL);

	if (netdev->flags & IFF_PROMISC) {
		rctl |= MAC_CTRL_PROMIS_EN;
	} else if (netdev->flags & IFF_ALLMULTI) {
		rctl |= MAC_CTRL_MC_ALL_EN;
		rctl &= ~MAC_CTRL_PROMIS_EN;
	} else
		rctl &= ~(MAC_CTRL_PROMIS_EN | MAC_CTRL_MC_ALL_EN);

	ATL2_WRITE_REG(hw, REG_MAC_CTRL, rctl);

	/* clear the old settings from the multicast hash table */
	ATL2_WRITE_REG(hw, REG_RX_HASH_TABLE, 0);
	ATL2_WRITE_REG_ARRAY(hw, REG_RX_HASH_TABLE, 1, 0);

	/* comoute mc addresses' hash value ,and put it into hash table */
	netdev_for_each_mc_addr(ha, netdev) {
		hash_value = atl2_hash_mc_addr(hw, ha->addr);
		atl2_hash_set(hw, hash_value);
	}
}
