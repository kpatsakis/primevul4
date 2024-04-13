static void cma_work_handler(struct work_struct *_work)
{
	struct cma_work *work = container_of(_work, struct cma_work, work);
	struct rdma_id_private *id_priv = work->id;
	int destroy = 0;

	mutex_lock(&id_priv->handler_mutex);
	if (!cma_comp_exch(id_priv, work->old_state, work->new_state))
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
