static inline struct sockaddr *cma_src_addr(struct rdma_id_private *id_priv)
{
	return (struct sockaddr *) &id_priv->id.route.addr.src_addr;
}
