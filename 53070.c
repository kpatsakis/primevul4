static inline void atl2_irq_disable(struct atl2_adapter *adapter)
{
    ATL2_WRITE_REG(&adapter->hw, REG_IMR, 0);
    ATL2_WRITE_FLUSH(&adapter->hw);
    synchronize_irq(adapter->pdev->irq);
}
