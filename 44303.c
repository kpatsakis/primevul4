static void cma_ndev_work_handler(struct work_struct *_work)
{
	struct cma_ndev_work *work = container_of(_work, struct cma_ndev_work, work);
	struct rdma_id_private *id_priv = work->id;
	int destroy = 0;

	mutex_lock(&id_priv->handler_mutex);
	if (id_priv->state == RDMA_CM_DESTROYING ||
	    id_priv->state == RDMA_CM_DEVICE_REMOVAL)
		goto out;

	if (id_priv->id.event_handler(&id_priv->id, &work->event)) {
		cma_exch(id_priv, RDMA_CM_DESTROYING);
		destroy = 1;
	}

out:
	mutex_unlock(&id_priv->handler_mutex);
	cma_deref_id(id_priv);
	if (destroy)
		rdma_destroy_id(&id_priv->id);
	kfree(work);
}
