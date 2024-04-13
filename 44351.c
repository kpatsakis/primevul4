int rdma_reject(struct rdma_cm_id *id, const void *private_data,
		u8 private_data_len)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!id_priv->cm_id.ib)
		return -EINVAL;

	switch (rdma_node_get_transport(id->device->node_type)) {
	case RDMA_TRANSPORT_IB:
		if (id->qp_type == IB_QPT_UD)
			ret = cma_send_sidr_rep(id_priv, IB_SIDR_REJECT, 0,
						private_data, private_data_len);
		else
			ret = ib_send_cm_rej(id_priv->cm_id.ib,
					     IB_CM_REJ_CONSUMER_DEFINED, NULL,
					     0, private_data, private_data_len);
		break;
	case RDMA_TRANSPORT_IWARP:
		ret = iw_cm_reject(id_priv->cm_id.iw,
				   private_data, private_data_len);
		break;
	default:
		ret = -ENOSYS;
		break;
	}
	return ret;
}
