static void addr_handler(int status, struct sockaddr *src_addr,
			 struct rdma_dev_addr *dev_addr, void *context)
{
	struct rdma_id_private *id_priv = context;
	struct rdma_cm_event event;

	memset(&event, 0, sizeof event);
	mutex_lock(&id_priv->handler_mutex);
	if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_QUERY,
			   RDMA_CM_ADDR_RESOLVED))
		goto out;

	memcpy(cma_src_addr(id_priv), src_addr, rdma_addr_size(src_addr));
	if (!status && !id_priv->cma_dev)
		status = cma_acquire_dev(id_priv, NULL);

	if (status) {
		if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_RESOLVED,
				   RDMA_CM_ADDR_BOUND))
			goto out;
		event.event = RDMA_CM_EVENT_ADDR_ERROR;
		event.status = status;
	} else
		event.event = RDMA_CM_EVENT_ADDR_RESOLVED;

	if (id_priv->id.event_handler(&id_priv->id, &event)) {
		cma_exch(id_priv, RDMA_CM_DESTROYING);
		mutex_unlock(&id_priv->handler_mutex);
		cma_deref_id(id_priv);
		rdma_destroy_id(&id_priv->id);
		return;
	}
out:
	mutex_unlock(&id_priv->handler_mutex);
	cma_deref_id(id_priv);
}
