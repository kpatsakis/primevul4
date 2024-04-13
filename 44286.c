static int cma_ib_handler(struct ib_cm_id *cm_id, struct ib_cm_event *ib_event)
{
	struct rdma_id_private *id_priv = cm_id->context;
	struct rdma_cm_event event;
	int ret = 0;

	if ((ib_event->event != IB_CM_TIMEWAIT_EXIT &&
		cma_disable_callback(id_priv, RDMA_CM_CONNECT)) ||
	    (ib_event->event == IB_CM_TIMEWAIT_EXIT &&
		cma_disable_callback(id_priv, RDMA_CM_DISCONNECT)))
		return 0;

	memset(&event, 0, sizeof event);
	switch (ib_event->event) {
	case IB_CM_REQ_ERROR:
	case IB_CM_REP_ERROR:
		event.event = RDMA_CM_EVENT_UNREACHABLE;
		event.status = -ETIMEDOUT;
		break;
	case IB_CM_REP_RECEIVED:
		if (id_priv->id.qp) {
			event.status = cma_rep_recv(id_priv);
			event.event = event.status ? RDMA_CM_EVENT_CONNECT_ERROR :
						     RDMA_CM_EVENT_ESTABLISHED;
		} else {
			event.event = RDMA_CM_EVENT_CONNECT_RESPONSE;
		}
		cma_set_rep_event_data(&event, &ib_event->param.rep_rcvd,
				       ib_event->private_data);
		break;
	case IB_CM_RTU_RECEIVED:
	case IB_CM_USER_ESTABLISHED:
		event.event = RDMA_CM_EVENT_ESTABLISHED;
		break;
	case IB_CM_DREQ_ERROR:
		event.status = -ETIMEDOUT; /* fall through */
	case IB_CM_DREQ_RECEIVED:
	case IB_CM_DREP_RECEIVED:
		if (!cma_comp_exch(id_priv, RDMA_CM_CONNECT,
				   RDMA_CM_DISCONNECT))
			goto out;
		event.event = RDMA_CM_EVENT_DISCONNECTED;
		break;
	case IB_CM_TIMEWAIT_EXIT:
		event.event = RDMA_CM_EVENT_TIMEWAIT_EXIT;
		break;
	case IB_CM_MRA_RECEIVED:
		/* ignore event */
		goto out;
	case IB_CM_REJ_RECEIVED:
		cma_modify_qp_err(id_priv);
		event.status = ib_event->param.rej_rcvd.reason;
		event.event = RDMA_CM_EVENT_REJECTED;
		event.param.conn.private_data = ib_event->private_data;
		event.param.conn.private_data_len = IB_CM_REJ_PRIVATE_DATA_SIZE;
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
