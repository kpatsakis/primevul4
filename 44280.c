static inline struct sockaddr *cma_dst_addr(struct rdma_id_private *id_priv)
{
	return (struct sockaddr *) &id_priv->id.route.addr.dst_addr;
}
