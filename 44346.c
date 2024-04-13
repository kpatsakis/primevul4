void rdma_destroy_qp(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv;

	id_priv = container_of(id, struct rdma_id_private, id);
	mutex_lock(&id_priv->qp_mutex);
	ib_destroy_qp(id_priv->id.qp);
	id_priv->id.qp = NULL;
	mutex_unlock(&id_priv->qp_mutex);
}
