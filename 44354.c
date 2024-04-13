int rdma_set_reuseaddr(struct rdma_cm_id *id, int reuse)
{
	struct rdma_id_private *id_priv;
	unsigned long flags;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	spin_lock_irqsave(&id_priv->lock, flags);
	if (reuse || id_priv->state == RDMA_CM_IDLE) {
		id_priv->reuseaddr = reuse;
		ret = 0;
	} else {
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&id_priv->lock, flags);
	return ret;
}
