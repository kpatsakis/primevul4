static inline void atl2_clear_phy_int(struct atl2_adapter *adapter)
{
	u16 phy_data;
	spin_lock(&adapter->stats_lock);
	atl2_read_phy_reg(&adapter->hw, 19, &phy_data);
	spin_unlock(&adapter->stats_lock);
}
