static int cma_iw_handler(struct iw_cm_id *iw_id, struct iw_cm_event *iw_event)
{
	struct rdma_id_private *id_priv = iw_id->context;
	struct rdma_cm_event event;
	int ret = 0;
	struct sockaddr *laddr = (struct sockaddr *)&iw_event->local_addr;
	struct sockaddr *raddr = (struct sockaddr *)&iw_event->remote_addr;

	if (cma_disable_callback(id_priv, RDMA_CM_CONNECT))
		return 0;

	memset(&event, 0, sizeof event);
	switch (iw_event->event) {
	case IW_CM_EVENT_CLOSE:
		event.event = RDMA_CM_EVENT_DISCONNECTED;
		break;
	case IW_CM_EVENT_CONNECT_REPLY:
		memcpy(cma_src_addr(id_priv), laddr,
		       rdma_addr_size(laddr));
		memcpy(cma_dst_addr(id_priv), raddr,
		       rdma_addr_size(raddr));
		switch (iw_event->status) {
		case 0:
			event.event = RDMA_CM_EVENT_ESTABLISHED;
			event.param.conn.initiator_depth = iw_event->ird;
			event.param.conn.responder_resources = iw_event->ord;
			break;
		case -ECONNRESET:
		case -ECONNREFUSED:
			event.event = RDMA_CM_EVENT_REJECTED;
			break;
		case -ETIMEDOUT:
			event.event = RDMA_CM_EVENT_UNREACHABLE;
			break;
		default:
			event.event = RDMA_CM_EVENT_CONNECT_ERROR;
			break;
		}
		break;
	case IW_CM_EVENT_ESTABLISHED:
		event.event = RDMA_CM_EVENT_ESTABLISHED;
		event.param.conn.initiator_depth = iw_event->ird;
		event.param.conn.responder_resources = iw_event->ord;
		break;
	default:
		BUG_ON(1);
	}

	event.status = iw_event->status;
	event.param.conn.private_data = iw_event->private_data;
	event.param.conn.private_data_len = iw_event->private_data_len;
	ret = id_priv->id.event_handler(&id_priv->id, &event);
	if (ret) {
		/* Destroy the CM ID by returning a non-zero value. */
		id_priv->cm_id.iw = NULL;
		cma_exch(id_priv, RDMA_CM_DESTROYING);
		mutex_unlock(&id_priv->handler_mutex);
		rdma_destroy_id(&id_priv->id);
		return ret;
	}

	mutex_unlock(&id_priv->handler_mutex);
	return ret;
}
