int rdma_disconnect(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!id_priv->cm_id.ib)
		return -EINVAL;

	switch (rdma_node_get_transport(id->device->node_type)) {
	case RDMA_TRANSPORT_IB:
		ret = cma_modify_qp_err(id_priv);
		if (ret)
			goto out;
		/* Initiate or respond to a disconnect. */
		if (ib_send_cm_dreq(id_priv->cm_id.ib, NULL, 0))
			ib_send_cm_drep(id_priv->cm_id.ib, NULL, 0);
		break;
	case RDMA_TRANSPORT_IWARP:
		ret = iw_cm_disconnect(id_priv->cm_id.iw, 0);
		break;
	default:
		ret = -EINVAL;
		break;
	}
out:
	return ret;
}
