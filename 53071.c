static inline void atl2_irq_enable(struct atl2_adapter *adapter)
{
	ATL2_WRITE_REG(&adapter->hw, REG_IMR, IMR_NORMAL_MASK);
	ATL2_WRITE_FLUSH(&adapter->hw);
}
