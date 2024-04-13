void rdma_destroy_id(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv;
	enum rdma_cm_state state;

	id_priv = container_of(id, struct rdma_id_private, id);
	state = cma_exch(id_priv, RDMA_CM_DESTROYING);
	cma_cancel_operation(id_priv, state);

	/*
	 * Wait for any active callback to finish.  New callbacks will find
	 * the id_priv state set to destroying and abort.
	 */
	mutex_lock(&id_priv->handler_mutex);
	mutex_unlock(&id_priv->handler_mutex);

	if (id_priv->cma_dev) {
		switch (rdma_node_get_transport(id_priv->id.device->node_type)) {
		case RDMA_TRANSPORT_IB:
			if (id_priv->cm_id.ib)
				ib_destroy_cm_id(id_priv->cm_id.ib);
			break;
		case RDMA_TRANSPORT_IWARP:
			if (id_priv->cm_id.iw)
				iw_destroy_cm_id(id_priv->cm_id.iw);
			break;
		default:
			break;
		}
		cma_leave_mc_groups(id_priv);
		cma_release_dev(id_priv);
	}

	cma_release_port(id_priv);
	cma_deref_id(id_priv);
	wait_for_completion(&id_priv->comp);

	if (id_priv->internal_id)
		cma_deref_id(id_priv->id.context);

	kfree(id_priv->id.route.path_rec);
	kfree(id_priv);
}
