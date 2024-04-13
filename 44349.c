int rdma_listen(struct rdma_cm_id *id, int backlog)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (id_priv->state == RDMA_CM_IDLE) {
		id->route.addr.src_addr.ss_family = AF_INET;
		ret = rdma_bind_addr(id, cma_src_addr(id_priv));
		if (ret)
			return ret;
	}

	if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_BOUND, RDMA_CM_LISTEN))
		return -EINVAL;

	if (id_priv->reuseaddr) {
		ret = cma_bind_listen(id_priv);
		if (ret)
			goto err;
	}

	id_priv->backlog = backlog;
	if (id->device) {
		switch (rdma_node_get_transport(id->device->node_type)) {
		case RDMA_TRANSPORT_IB:
			ret = cma_ib_listen(id_priv);
			if (ret)
				goto err;
			break;
		case RDMA_TRANSPORT_IWARP:
			ret = cma_iw_listen(id_priv, backlog);
			if (ret)
				goto err;
			break;
		default:
			ret = -ENOSYS;
			goto err;
		}
	} else
		cma_listen_on_all(id_priv);

	return 0;
err:
	id_priv->backlog = 0;
	cma_comp_exch(id_priv, RDMA_CM_LISTEN, RDMA_CM_ADDR_BOUND);
	return ret;
}
