static void atl2_reset_task(struct work_struct *work)
{
	struct atl2_adapter *adapter;
	adapter = container_of(work, struct atl2_adapter, reset_task);

	atl2_reinit_locked(adapter);
}
