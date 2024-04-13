static void cm_free_id(__be32 local_id)
{
	spin_lock_irq(&cm.lock);
	idr_remove(&cm.local_id_table,
		   (__force int) (local_id ^ cm.random_id_operand));
	spin_unlock_irq(&cm.lock);
}
