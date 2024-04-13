static struct rdma_id_private *cma_new_conn_id(struct rdma_cm_id *listen_id,
					       struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv;
	struct rdma_cm_id *id;
	struct rdma_route *rt;
	int ret;

	id = rdma_create_id(listen_id->event_handler, listen_id->context,
			    listen_id->ps, ib_event->param.req_rcvd.qp_type);
	if (IS_ERR(id))
		return NULL;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (cma_save_net_info(id, listen_id, ib_event))
		goto err;

	rt = &id->route;
	rt->num_paths = ib_event->param.req_rcvd.alternate_path ? 2 : 1;
	rt->path_rec = kmalloc(sizeof *rt->path_rec * rt->num_paths,
			       GFP_KERNEL);
	if (!rt->path_rec)
		goto err;

	rt->path_rec[0] = *ib_event->param.req_rcvd.primary_path;
	if (rt->num_paths == 2)
		rt->path_rec[1] = *ib_event->param.req_rcvd.alternate_path;

	if (cma_any_addr(cma_src_addr(id_priv))) {
		rt->addr.dev_addr.dev_type = ARPHRD_INFINIBAND;
		rdma_addr_set_sgid(&rt->addr.dev_addr, &rt->path_rec[0].sgid);
		ib_addr_set_pkey(&rt->addr.dev_addr, be16_to_cpu(rt->path_rec[0].pkey));
	} else {
		ret = cma_translate_addr(cma_src_addr(id_priv), &rt->addr.dev_addr);
		if (ret)
			goto err;
	}
	rdma_addr_set_dgid(&rt->addr.dev_addr, &rt->path_rec[0].dgid);

	id_priv->state = RDMA_CM_CONNECT;
	return id_priv;

err:
	rdma_destroy_id(id);
	return NULL;
}
