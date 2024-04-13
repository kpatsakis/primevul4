static inline unsigned short cma_family(struct rdma_id_private *id_priv)
{
	return id_priv->id.route.addr.src_addr.ss_family;
}
