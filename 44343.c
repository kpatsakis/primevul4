int rdma_connect(struct rdma_cm_id *id, struct rdma_conn_param *conn_param)
{
	struct rdma_id_private *id_priv;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!cma_comp_exch(id_priv, RDMA_CM_ROUTE_RESOLVED, RDMA_CM_CONNECT))
		return -EINVAL;

	if (!id->qp) {
		id_priv->qp_num = conn_param->qp_num;
		id_priv->srq = conn_param->srq;
	}

	switch (rdma_node_get_transport(id->device->node_type)) {
	case RDMA_TRANSPORT_IB:
		if (id->qp_type == IB_QPT_UD)
			ret = cma_resolve_ib_udp(id_priv, conn_param);
		else
			ret = cma_connect_ib(id_priv, conn_param);
		break;
	case RDMA_TRANSPORT_IWARP:
		ret = cma_connect_iw(id_priv, conn_param);
		break;
	default:
		ret = -ENOSYS;
		break;
	}
	if (ret)
		goto err;

	return 0;
err:
	cma_comp_exch(id_priv, RDMA_CM_CONNECT, RDMA_CM_ROUTE_RESOLVED);
	return ret;
}
