static int cma_modify_qp_rtr(struct rdma_id_private *id_priv,
			     struct rdma_conn_param *conn_param)
{
	struct ib_qp_attr qp_attr;
	int qp_attr_mask, ret;
	union ib_gid sgid;

	mutex_lock(&id_priv->qp_mutex);
	if (!id_priv->id.qp) {
		ret = 0;
		goto out;
	}

	/* Need to update QP attributes from default values. */
	qp_attr.qp_state = IB_QPS_INIT;
	ret = rdma_init_qp_attr(&id_priv->id, &qp_attr, &qp_attr_mask);
	if (ret)
		goto out;

	ret = ib_modify_qp(id_priv->id.qp, &qp_attr, qp_attr_mask);
	if (ret)
		goto out;

	qp_attr.qp_state = IB_QPS_RTR;
	ret = rdma_init_qp_attr(&id_priv->id, &qp_attr, &qp_attr_mask);
	if (ret)
		goto out;

	ret = ib_query_gid(id_priv->id.device, id_priv->id.port_num,
			   qp_attr.ah_attr.grh.sgid_index, &sgid);
	if (ret)
		goto out;

	if (rdma_node_get_transport(id_priv->cma_dev->device->node_type)
	    == RDMA_TRANSPORT_IB &&
	    rdma_port_get_link_layer(id_priv->id.device, id_priv->id.port_num)
	    == IB_LINK_LAYER_ETHERNET) {
		ret = rdma_addr_find_smac_by_sgid(&sgid, qp_attr.smac, NULL);

		if (ret)
			goto out;
	}
	if (conn_param)
		qp_attr.max_dest_rd_atomic = conn_param->responder_resources;
	ret = ib_modify_qp(id_priv->id.qp, &qp_attr, qp_attr_mask);
out:
	mutex_unlock(&id_priv->qp_mutex);
	return ret;
}
