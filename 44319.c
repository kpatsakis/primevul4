static int cma_resolve_iw_route(struct rdma_id_private *id_priv, int timeout_ms)
{
	struct cma_work *work;

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (!work)
		return -ENOMEM;

	work->id = id_priv;
	INIT_WORK(&work->work, cma_work_handler);
	work->old_state = RDMA_CM_ROUTE_QUERY;
	work->new_state = RDMA_CM_ROUTE_RESOLVED;
	work->event.event = RDMA_CM_EVENT_ROUTE_RESOLVED;
	queue_work(cma_wq, &work->work);
	return 0;
}
