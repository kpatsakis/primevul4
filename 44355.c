void rdma_set_service_type(struct rdma_cm_id *id, int tos)
{
	struct rdma_id_private *id_priv;

	id_priv = container_of(id, struct rdma_id_private, id);
	id_priv->tos = (u8) tos;
}
