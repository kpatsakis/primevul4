int rdma_notify(struct rdma_cm_id *id, enum ib_event_type event)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!id_priv->cm_id.ib)
		return -EINVAL;

	switch (id->device->node_type) {
	case RDMA_NODE_IB_CA:
		ret = ib_cm_notify(id_priv->cm_id.ib, event);
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
}
