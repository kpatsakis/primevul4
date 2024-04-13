static int cma_remove_id_dev(struct rdma_id_private *id_priv)
{
	struct rdma_cm_event event;
	enum rdma_cm_state state;
	int ret = 0;

	/* Record that we want to remove the device */
	state = cma_exch(id_priv, RDMA_CM_DEVICE_REMOVAL);
	if (state == RDMA_CM_DESTROYING)
		return 0;

	cma_cancel_operation(id_priv, state);
	mutex_lock(&id_priv->handler_mutex);

	/* Check for destruction from another callback. */
	if (!cma_comp(id_priv, RDMA_CM_DEVICE_REMOVAL))
		goto out;

	memset(&event, 0, sizeof event);
	event.event = RDMA_CM_EVENT_DEVICE_REMOVAL;
	ret = id_priv->id.event_handler(&id_priv->id, &event);
out:
	mutex_unlock(&id_priv->handler_mutex);
	return ret;
}
