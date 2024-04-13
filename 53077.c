static void atl2_phy_config(unsigned long data)
{
	struct atl2_adapter *adapter = (struct atl2_adapter *) data;
	struct atl2_hw *hw = &adapter->hw;
	unsigned long flags;

	spin_lock_irqsave(&adapter->stats_lock, flags);
	atl2_write_phy_reg(hw, MII_ADVERTISE, hw->mii_autoneg_adv_reg);
	atl2_write_phy_reg(hw, MII_BMCR, MII_CR_RESET | MII_CR_AUTO_NEG_EN |
		MII_CR_RESTART_AUTO_NEG);
	spin_unlock_irqrestore(&adapter->stats_lock, flags);
	clear_bit(0, &adapter->cfg_phy);
}
