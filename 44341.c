static int iw_conn_req_handler(struct iw_cm_id *cm_id,
			       struct iw_cm_event *iw_event)
{
	struct rdma_cm_id *new_cm_id;
	struct rdma_id_private *listen_id, *conn_id;
	struct rdma_cm_event event;
	int ret;
	struct ib_device_attr attr;
	struct sockaddr *laddr = (struct sockaddr *)&iw_event->local_addr;
	struct sockaddr *raddr = (struct sockaddr *)&iw_event->remote_addr;

	listen_id = cm_id->context;
	if (cma_disable_callback(listen_id, RDMA_CM_LISTEN))
		return -ECONNABORTED;

	/* Create a new RDMA id for the new IW CM ID */
	new_cm_id = rdma_create_id(listen_id->id.event_handler,
				   listen_id->id.context,
				   RDMA_PS_TCP, IB_QPT_RC);
	if (IS_ERR(new_cm_id)) {
		ret = -ENOMEM;
		goto out;
	}
	conn_id = container_of(new_cm_id, struct rdma_id_private, id);
	mutex_lock_nested(&conn_id->handler_mutex, SINGLE_DEPTH_NESTING);
	conn_id->state = RDMA_CM_CONNECT;

	ret = rdma_translate_ip(laddr, &conn_id->id.route.addr.dev_addr, NULL);
	if (ret) {
		mutex_unlock(&conn_id->handler_mutex);
		rdma_destroy_id(new_cm_id);
		goto out;
	}

	ret = cma_acquire_dev(conn_id, listen_id);
	if (ret) {
		mutex_unlock(&conn_id->handler_mutex);
		rdma_destroy_id(new_cm_id);
		goto out;
	}

	conn_id->cm_id.iw = cm_id;
	cm_id->context = conn_id;
	cm_id->cm_handler = cma_iw_handler;

	memcpy(cma_src_addr(conn_id), laddr, rdma_addr_size(laddr));
	memcpy(cma_dst_addr(conn_id), raddr, rdma_addr_size(raddr));

	ret = ib_query_device(conn_id->id.device, &attr);
	if (ret) {
		mutex_unlock(&conn_id->handler_mutex);
		rdma_destroy_id(new_cm_id);
		goto out;
	}

	memset(&event, 0, sizeof event);
	event.event = RDMA_CM_EVENT_CONNECT_REQUEST;
	event.param.conn.private_data = iw_event->private_data;
	event.param.conn.private_data_len = iw_event->private_data_len;
	event.param.conn.initiator_depth = iw_event->ird;
	event.param.conn.responder_resources = iw_event->ord;

	/*
	 * Protect against the user destroying conn_id from another thread
	 * until we're done accessing it.
	 */
	atomic_inc(&conn_id->refcount);
	ret = conn_id->id.event_handler(&conn_id->id, &event);
	if (ret) {
		/* User wants to destroy the CM ID */
		conn_id->cm_id.iw = NULL;
		cma_exch(conn_id, RDMA_CM_DESTROYING);
		mutex_unlock(&conn_id->handler_mutex);
		cma_deref_id(conn_id);
		rdma_destroy_id(&conn_id->id);
		goto out;
	}

	mutex_unlock(&conn_id->handler_mutex);
	cma_deref_id(conn_id);

out:
	mutex_unlock(&listen_id->handler_mutex);
	return ret;
}
