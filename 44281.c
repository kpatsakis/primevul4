static enum rdma_cm_state cma_exch(struct rdma_id_private *id_priv,
				   enum rdma_cm_state exch)
{
	unsigned long flags;
	enum rdma_cm_state old;

	spin_lock_irqsave(&id_priv->lock, flags);
	old = id_priv->state;
	id_priv->state = exch;
	spin_unlock_irqrestore(&id_priv->lock, flags);
	return old;
}
