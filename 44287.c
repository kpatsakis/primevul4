static int cma_ib_listen(struct rdma_id_private *id_priv)
{
	struct ib_cm_compare_data compare_data;
	struct sockaddr *addr;
	struct ib_cm_id	*id;
	__be64 svc_id;
	int ret;

	id = ib_create_cm_id(id_priv->id.device, cma_req_handler, id_priv);
	if (IS_ERR(id))
		return PTR_ERR(id);

	id_priv->cm_id.ib = id;

	addr = cma_src_addr(id_priv);
	svc_id = rdma_get_service_id(&id_priv->id, addr);
	if (cma_any_addr(addr) && !id_priv->afonly)
		ret = ib_cm_listen(id_priv->cm_id.ib, svc_id, 0, NULL);
	else {
		cma_set_compare_data(id_priv->id.ps, addr, &compare_data);
		ret = ib_cm_listen(id_priv->cm_id.ib, svc_id, 0, &compare_data);
	}

	if (ret) {
		ib_destroy_cm_id(id_priv->cm_id.ib);
		id_priv->cm_id.ib = NULL;
	}

	return ret;
}
