static struct idr *cma_select_inet_ps(struct rdma_id_private *id_priv)
{
	switch (id_priv->id.ps) {
	case RDMA_PS_TCP:
		return &tcp_ps;
	case RDMA_PS_UDP:
		return &udp_ps;
	case RDMA_PS_IPOIB:
		return &ipoib_ps;
	case RDMA_PS_IB:
		return &ib_ps;
	default:
		return NULL;
	}
}
