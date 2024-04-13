static void atl2_link_chg_task(struct work_struct *work)
{
	struct atl2_adapter *adapter;
	unsigned long flags;

	adapter = container_of(work, struct atl2_adapter, link_chg_task);

	spin_lock_irqsave(&adapter->stats_lock, flags);
	atl2_check_link(adapter);
	spin_unlock_irqrestore(&adapter->stats_lock, flags);
}
