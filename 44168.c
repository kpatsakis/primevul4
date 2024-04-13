static int cm_alloc_id(struct cm_id_private *cm_id_priv)
{
	unsigned long flags;
	int id;

	idr_preload(GFP_KERNEL);
	spin_lock_irqsave(&cm.lock, flags);

	id = idr_alloc_cyclic(&cm.local_id_table, cm_id_priv, 0, 0, GFP_NOWAIT);

	spin_unlock_irqrestore(&cm.lock, flags);
	idr_preload_end();

	cm_id_priv->id.local_id = (__force __be32)id ^ cm.random_id_operand;
	return id < 0 ? id : 0;
}
