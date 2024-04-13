static int cm_sidr_req_handler(struct cm_work *work)
{
	struct ib_cm_id *cm_id;
	struct cm_id_private *cm_id_priv, *cur_cm_id_priv;
	struct cm_sidr_req_msg *sidr_req_msg;
	struct ib_wc *wc;

	cm_id = ib_create_cm_id(work->port->cm_dev->ib_device, NULL, NULL);
	if (IS_ERR(cm_id))
		return PTR_ERR(cm_id);
	cm_id_priv = container_of(cm_id, struct cm_id_private, id);

	/* Record SGID/SLID and request ID for lookup. */
	sidr_req_msg = (struct cm_sidr_req_msg *)
				work->mad_recv_wc->recv_buf.mad;
	wc = work->mad_recv_wc->wc;
	cm_id_priv->av.dgid.global.subnet_prefix = cpu_to_be64(wc->slid);
	cm_id_priv->av.dgid.global.interface_id = 0;
	cm_init_av_for_response(work->port, work->mad_recv_wc->wc,
				work->mad_recv_wc->recv_buf.grh,
				&cm_id_priv->av);
	cm_id_priv->id.remote_id = sidr_req_msg->request_id;
	cm_id_priv->tid = sidr_req_msg->hdr.tid;
	atomic_inc(&cm_id_priv->work_count);

	spin_lock_irq(&cm.lock);
	cur_cm_id_priv = cm_insert_remote_sidr(cm_id_priv);
	if (cur_cm_id_priv) {
		spin_unlock_irq(&cm.lock);
		atomic_long_inc(&work->port->counter_group[CM_RECV_DUPLICATES].
				counter[CM_SIDR_REQ_COUNTER]);
		goto out; /* Duplicate message. */
	}
	cm_id_priv->id.state = IB_CM_SIDR_REQ_RCVD;
	cur_cm_id_priv = cm_find_listen(cm_id->device,
					sidr_req_msg->service_id,
					sidr_req_msg->private_data);
	if (!cur_cm_id_priv) {
		spin_unlock_irq(&cm.lock);
		cm_reject_sidr_req(cm_id_priv, IB_SIDR_UNSUPPORTED);
		goto out; /* No match. */
	}
	atomic_inc(&cur_cm_id_priv->refcount);
	atomic_inc(&cm_id_priv->refcount);
	spin_unlock_irq(&cm.lock);

	cm_id_priv->id.cm_handler = cur_cm_id_priv->id.cm_handler;
	cm_id_priv->id.context = cur_cm_id_priv->id.context;
	cm_id_priv->id.service_id = sidr_req_msg->service_id;
	cm_id_priv->id.service_mask = ~cpu_to_be64(0);

	cm_format_sidr_req_event(work, &cur_cm_id_priv->id);
	cm_process_work(cm_id_priv, work);
	cm_deref_id(cur_cm_id_priv);
	return 0;
out:
	ib_destroy_cm_id(&cm_id_priv->id);
	return -EINVAL;
}
