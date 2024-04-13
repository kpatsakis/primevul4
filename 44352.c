int rdma_set_afonly(struct rdma_cm_id *id, int afonly)
{
	struct rdma_id_private *id_priv;
	unsigned long flags;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	spin_lock_irqsave(&id_priv->lock, flags);
	if (id_priv->state == RDMA_CM_IDLE || id_priv->state == RDMA_CM_ADDR_BOUND) {
		id_priv->options |= (1 << CMA_OPTION_AFONLY);
		id_priv->afonly = afonly;
		ret = 0;
	} else {
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&id_priv->lock, flags);
	return ret;
}
