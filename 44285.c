static int cma_get_port(struct rdma_id_private *id_priv)
{
	struct idr *ps;
	int ret;

	if (cma_family(id_priv) != AF_IB)
		ps = cma_select_inet_ps(id_priv);
	else
		ps = cma_select_ib_ps(id_priv);
	if (!ps)
		return -EPROTONOSUPPORT;

	mutex_lock(&lock);
	if (cma_any_port(cma_src_addr(id_priv)))
		ret = cma_alloc_any_port(ps, id_priv);
	else
		ret = cma_use_port(ps, id_priv);
	mutex_unlock(&lock);

	return ret;
}
