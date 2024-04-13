static int cma_disable_callback(struct rdma_id_private *id_priv,
				enum rdma_cm_state state)
{
	mutex_lock(&id_priv->handler_mutex);
	if (id_priv->state != state) {
		mutex_unlock(&id_priv->handler_mutex);
		return -EINVAL;
	}
	return 0;
}
