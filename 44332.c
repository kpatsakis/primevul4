static int cma_sidr_rep_handler(struct ib_cm_id *cm_id,
				struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv = cm_id->context;
	struct rdma_cm_event event;
	struct ib_cm_sidr_rep_event_param *rep = &ib_event->param.sidr_rep_rcvd;
	int ret = 0;

	if (cma_disable_callback(id_priv, RDMA_CM_CONNECT))
		return 0;

	memset(&event, 0, sizeof event);
	switch (ib_event->event) {
	case IB_CM_SIDR_REQ_ERROR:
		event.event = RDMA_CM_EVENT_UNREACHABLE;
		event.status = -ETIMEDOUT;
		break;
	case IB_CM_SIDR_REP_RECEIVED:
		event.param.ud.private_data = ib_event->private_data;
		event.param.ud.private_data_len = IB_CM_SIDR_REP_PRIVATE_DATA_SIZE;
		if (rep->status != IB_SIDR_SUCCESS) {
			event.event = RDMA_CM_EVENT_UNREACHABLE;
			event.status = ib_event->param.sidr_rep_rcvd.status;
			break;
		}
		ret = cma_set_qkey(id_priv, rep->qkey);
		if (ret) {
			event.event = RDMA_CM_EVENT_ADDR_ERROR;
			event.status = ret;
			break;
		}
		ib_init_ah_from_path(id_priv->id.device, id_priv->id.port_num,
				     id_priv->id.route.path_rec,
				     &event.param.ud.ah_attr);
		event.param.ud.qp_num = rep->qpn;
		event.param.ud.qkey = rep->qkey;
		event.event = RDMA_CM_EVENT_ESTABLISHED;
		event.status = 0;
		break;
	default:
		printk(KERN_ERR "RDMA CMA: unexpected IB CM event: %d\n",
		       ib_event->event);
		goto out;
	}

	ret = id_priv->id.event_handler(&id_priv->id, &event);
	if (ret) {
		/* Destroy the CM ID by returning a non-zero value. */
		id_priv->cm_id.ib = NULL;
		cma_exch(id_priv, RDMA_CM_DESTROYING);
		mutex_unlock(&id_priv->handler_mutex);
		rdma_destroy_id(&id_priv->id);
		return ret;
	}
out:
	mutex_unlock(&id_priv->handler_mutex);
	return ret;
}
