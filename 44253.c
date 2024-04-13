int ib_send_cm_req(struct ib_cm_id *cm_id,
		   struct ib_cm_req_param *param)
{
	struct cm_id_private *cm_id_priv;
	struct cm_req_msg *req_msg;
	unsigned long flags;
	int ret;

	ret = cm_validate_req_param(param);
	if (ret)
		return ret;

	/* Verify that we're not in timewait. */
	cm_id_priv = container_of(cm_id, struct cm_id_private, id);
	spin_lock_irqsave(&cm_id_priv->lock, flags);
	if (cm_id->state != IB_CM_IDLE) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		ret = -EINVAL;
		goto out;
	}
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);

	cm_id_priv->timewait_info = cm_create_timewait_info(cm_id_priv->
							    id.local_id);
	if (IS_ERR(cm_id_priv->timewait_info)) {
		ret = PTR_ERR(cm_id_priv->timewait_info);
		goto out;
	}

	ret = cm_init_av_by_path(param->primary_path, &cm_id_priv->av);
	if (ret)
		goto error1;
	if (param->alternate_path) {
		ret = cm_init_av_by_path(param->alternate_path,
					 &cm_id_priv->alt_av);
		if (ret)
			goto error1;
	}
	cm_id->service_id = param->service_id;
	cm_id->service_mask = ~cpu_to_be64(0);
	cm_id_priv->timeout_ms = cm_convert_to_ms(
				    param->primary_path->packet_life_time) * 2 +
				 cm_convert_to_ms(
				    param->remote_cm_response_timeout);
	cm_id_priv->max_cm_retries = param->max_cm_retries;
	cm_id_priv->initiator_depth = param->initiator_depth;
	cm_id_priv->responder_resources = param->responder_resources;
	cm_id_priv->retry_count = param->retry_count;
	cm_id_priv->path_mtu = param->primary_path->mtu;
	cm_id_priv->pkey = param->primary_path->pkey;
	cm_id_priv->qp_type = param->qp_type;

	ret = cm_alloc_msg(cm_id_priv, &cm_id_priv->msg);
	if (ret)
		goto error1;

	req_msg = (struct cm_req_msg *) cm_id_priv->msg->mad;
	cm_format_req(req_msg, cm_id_priv, param);
	cm_id_priv->tid = req_msg->hdr.tid;
	cm_id_priv->msg->timeout_ms = cm_id_priv->timeout_ms;
	cm_id_priv->msg->context[1] = (void *) (unsigned long) IB_CM_REQ_SENT;

	cm_id_priv->local_qpn = cm_req_get_local_qpn(req_msg);
	cm_id_priv->rq_psn = cm_req_get_starting_psn(req_msg);

	spin_lock_irqsave(&cm_id_priv->lock, flags);
	ret = ib_post_send_mad(cm_id_priv->msg, NULL);
	if (ret) {
		spin_unlock_irqrestore(&cm_id_priv->lock, flags);
		goto error2;
	}
	BUG_ON(cm_id->state != IB_CM_IDLE);
	cm_id->state = IB_CM_REQ_SENT;
	spin_unlock_irqrestore(&cm_id_priv->lock, flags);
	return 0;

error2:	cm_free_msg(cm_id_priv->msg);
error1:	kfree(cm_id_priv->timewait_info);
out:	return ret;
}
