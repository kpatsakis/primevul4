static int cma_ib_mc_handler(int status, struct ib_sa_multicast *multicast)
{
	struct rdma_id_private *id_priv;
	struct cma_multicast *mc = multicast->context;
	struct rdma_cm_event event;
	int ret;

	id_priv = mc->id_priv;
	if (cma_disable_callback(id_priv, RDMA_CM_ADDR_BOUND) &&
	    cma_disable_callback(id_priv, RDMA_CM_ADDR_RESOLVED))
		return 0;

	if (!status)
		status = cma_set_qkey(id_priv, be32_to_cpu(multicast->rec.qkey));
	mutex_lock(&id_priv->qp_mutex);
	if (!status && id_priv->id.qp)
		status = ib_attach_mcast(id_priv->id.qp, &multicast->rec.mgid,
					 be16_to_cpu(multicast->rec.mlid));
	mutex_unlock(&id_priv->qp_mutex);

	memset(&event, 0, sizeof event);
	event.status = status;
	event.param.ud.private_data = mc->context;
	if (!status) {
		event.event = RDMA_CM_EVENT_MULTICAST_JOIN;
		ib_init_ah_from_mcmember(id_priv->id.device,
					 id_priv->id.port_num, &multicast->rec,
					 &event.param.ud.ah_attr);
		event.param.ud.qp_num = 0xFFFFFF;
		event.param.ud.qkey = be32_to_cpu(multicast->rec.qkey);
	} else
		event.event = RDMA_CM_EVENT_MULTICAST_ERROR;

	ret = id_priv->id.event_handler(&id_priv->id, &event);
	if (ret) {
		cma_exch(id_priv, RDMA_CM_DESTROYING);
		mutex_unlock(&id_priv->handler_mutex);
		rdma_destroy_id(&id_priv->id);
		return 0;
	}

	mutex_unlock(&id_priv->handler_mutex);
	return 0;
}
