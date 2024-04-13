int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr)
{
	struct rdma_id_private *id_priv;
	int ret;

	if (addr->sa_family != AF_INET && addr->sa_family != AF_INET6 &&
	    addr->sa_family != AF_IB)
		return -EAFNOSUPPORT;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!cma_comp_exch(id_priv, RDMA_CM_IDLE, RDMA_CM_ADDR_BOUND))
		return -EINVAL;

	ret = cma_check_linklocal(&id->route.addr.dev_addr, addr);
	if (ret)
		goto err1;

	memcpy(cma_src_addr(id_priv), addr, rdma_addr_size(addr));
	if (!cma_any_addr(addr)) {
		ret = cma_translate_addr(addr, &id->route.addr.dev_addr);
		if (ret)
			goto err1;

		ret = cma_acquire_dev(id_priv, NULL);
		if (ret)
			goto err1;
	}

	if (!(id_priv->options & (1 << CMA_OPTION_AFONLY))) {
		if (addr->sa_family == AF_INET)
			id_priv->afonly = 1;
#if IS_ENABLED(CONFIG_IPV6)
		else if (addr->sa_family == AF_INET6)
			id_priv->afonly = init_net.ipv6.sysctl.bindv6only;
#endif
	}
	ret = cma_get_port(id_priv);
	if (ret)
		goto err2;

	return 0;
err2:
	if (id_priv->cma_dev)
		cma_release_dev(id_priv);
err1:
	cma_comp_exch(id_priv, RDMA_CM_ADDR_BOUND, RDMA_CM_IDLE);
	return ret;
}
