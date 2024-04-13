static int cm_req_handler(struct cm_work *work)
{
	struct ib_cm_id *cm_id;
	struct cm_id_private *cm_id_priv, *listen_cm_id_priv;
	struct cm_req_msg *req_msg;
	int ret;

	req_msg = (struct cm_req_msg *)work->mad_recv_wc->recv_buf.mad;

	cm_id = ib_create_cm_id(work->port->cm_dev->ib_device, NULL, NULL);
	if (IS_ERR(cm_id))
		return PTR_ERR(cm_id);

	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	cm_id_priv->id.remote_id = req_msg->local_comm_id;
	cm_init_av_for_response(work->port, work->mad_recv_wc->wc,
				work->mad_recv_wc->recv_buf.grh,
				&cm_id_priv->av);
	cm_id_priv->timewait_info = cm_create_timewait_info(cm_id_priv->
							    id.local_id);
	if (IS_ERR(cm_id_priv->timewait_info)) {
		ret = PTR_ERR(cm_id_priv->timewait_info);
		goto destroy;
	}
	cm_id_priv->timewait_info->work.remote_id = req_msg->local_comm_id;
	cm_id_priv->timewait_info->remote_ca_guid = req_msg->local_ca_guid;
	cm_id_priv->timewait_info->remote_qpn = cm_req_get_local_qpn(req_msg);

	listen_cm_id_priv = cm_match_req(work, cm_id_priv);
	if (!listen_cm_id_priv) {
		ret = -EINVAL;
		kfree(cm_id_priv->timewait_info);
		goto destroy;
	}

	cm_id_priv->id.cm_handler = listen_cm_id_priv->id.cm_handler;
	cm_id_priv->id.context = listen_cm_id_priv->id.context;
	cm_id_priv->id.service_id = req_msg->service_id;
	cm_id_priv->id.service_mask = ~cpu_to_be64(0);

	cm_process_routed_req(req_msg, work->mad_recv_wc->wc);
	cm_format_paths_from_req(req_msg, &work->path[0], &work->path[1]);

	memcpy(work->path[0].dmac, cm_id_priv->av.ah_attr.dmac, ETH_ALEN);
	work->path[0].vlan_id = cm_id_priv->av.ah_attr.vlan_id;
	ret = cm_init_av_by_path(&work->path[0], &cm_id_priv->av);
	if (ret) {
		ib_get_cached_gid(work->port->cm_dev->ib_device,
				  work->port->port_num, 0, &work->path[0].sgid);
		ib_send_cm_rej(cm_id, IB_CM_REJ_INVALID_GID,
			       &work->path[0].sgid, sizeof work->path[0].sgid,
			       NULL, 0);
		goto rejected;
	}
	if (req_msg->alt_local_lid) {
		ret = cm_init_av_by_path(&work->path[1], &cm_id_priv->alt_av);
		if (ret) {
			ib_send_cm_rej(cm_id, IB_CM_REJ_INVALID_ALT_GID,
				       &work->path[0].sgid,
				       sizeof work->path[0].sgid, NULL, 0);
			goto rejected;
		}
	}
	cm_id_priv->tid = req_msg->hdr.tid;
	cm_id_priv->timeout_ms = cm_convert_to_ms(
					cm_req_get_local_resp_timeout(req_msg));
	cm_id_priv->max_cm_retries = cm_req_get_max_cm_retries(req_msg);
	cm_id_priv->remote_qpn = cm_req_get_local_qpn(req_msg);
	cm_id_priv->initiator_depth = cm_req_get_resp_res(req_msg);
	cm_id_priv->responder_resources = cm_req_get_init_depth(req_msg);
	cm_id_priv->path_mtu = cm_req_get_path_mtu(req_msg);
	cm_id_priv->pkey = req_msg->pkey;
	cm_id_priv->sq_psn = cm_req_get_starting_psn(req_msg);
	cm_id_priv->retry_count = cm_req_get_retry_count(req_msg);
	cm_id_priv->rnr_retry_count = cm_req_get_rnr_retry_count(req_msg);
	cm_id_priv->qp_type = cm_req_get_qp_type(req_msg);

	cm_format_req_event(work, cm_id_priv, &listen_cm_id_priv->id);
	cm_process_work(cm_id_priv, work);
	cm_deref_id(listen_cm_id_priv);
	return 0;

rejected:
	atomic_dec(&cm_id_priv->refcount);
	cm_deref_id(listen_cm_id_priv);
destroy:
	ib_destroy_cm_id(cm_id);
	return ret;
}
