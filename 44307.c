static struct rdma_id_private *cma_new_udp_id(struct rdma_cm_id *listen_id,
					      struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv;
	struct rdma_cm_id *id;
	int ret;

	id = rdma_create_id(listen_id->event_handler, listen_id->context,
			    listen_id->ps, IB_QPT_UD);
	if (IS_ERR(id))
		return NULL;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (cma_save_net_info(id, listen_id, ib_event))
		goto err;

	if (!cma_any_addr((struct sockaddr *) &id->route.addr.src_addr)) {
		ret = cma_translate_addr(cma_src_addr(id_priv), &id->route.addr.dev_addr);
		if (ret)
			goto err;
	}

	id_priv->state = RDMA_CM_CONNECT;
	return id_priv;
err:
	rdma_destroy_id(id);
	return NULL;
}
