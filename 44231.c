static int cm_rep_handler(struct cm_work *work)
{
	struct cm_id_private *cm_id_priv;
	struct cm_rep_msg *rep_msg;
	int ret;

	rep_msg = (struct cm_rep_msg *)work->mad_recv_wc->recv_buf.mad;
	cm_id_priv = cm_acquire_id(rep_msg->remote_comm_id, 0);
	if (!cm_id_priv) {
		cm_dup_rep_handler(work);
		return -EINVAL;
	}

	cm_format_rep_event(work, cm_id_priv->qp_type);

	spin_lock_irq(&cm_id_priv->lock);
	switch (cm_id_priv->id.state) {
	case IB_CM_REQ_SENT:
	case IB_CM_MRA_REQ_RCVD:
		break;
	default:
		spin_unlock_irq(&cm_id_priv->lock);
		ret = -EINVAL;
		goto error;
	}

	cm_id_priv->timewait_info->work.remote_id = rep_msg->local_comm_id;
	cm_id_priv->timewait_info->remote_ca_guid = rep_msg->local_ca_guid;
	cm_id_priv->timewait_info->remote_qpn = cm_rep_get_qpn(rep_msg, cm_id_priv->qp_type);

	spin_lock(&cm.lock);
	/* Check for duplicate REP. */
	if (cm_insert_remote_id(cm_id_priv->timewait_info)) {
		spin_unlock(&cm.lock);
		spin_unlock_irq(&cm_id_priv->lock);
		ret = -EINVAL;
		goto error;
	}
	/* Check for a stale connection. */
	if (cm_insert_remote_qpn(cm_id_priv->timewait_info)) {
		rb_erase(&cm_id_priv->timewait_info->remote_id_node,
			 &cm.remote_id_table);
		cm_id_priv->timewait_info->inserted_remote_id = 0;
		spin_unlock(&cm.lock);
		spin_unlock_irq(&cm_id_priv->lock);
		cm_issue_rej(work->port, work->mad_recv_wc,
			     IB_CM_REJ_STALE_CONN, CM_MSG_RESPONSE_REP,
			     NULL, 0);
		ret = -EINVAL;
		goto error;
	}
	spin_unlock(&cm.lock);

	cm_id_priv->id.state = IB_CM_REP_RCVD;
	cm_id_priv->id.remote_id = rep_msg->local_comm_id;
	cm_id_priv->remote_qpn = cm_rep_get_qpn(rep_msg, cm_id_priv->qp_type);
	cm_id_priv->initiator_depth = rep_msg->resp_resources;
	cm_id_priv->responder_resources = rep_msg->initiator_depth;
	cm_id_priv->sq_psn = cm_rep_get_starting_psn(rep_msg);
	cm_id_priv->rnr_retry_count = cm_rep_get_rnr_retry_count(rep_msg);
	cm_id_priv->target_ack_delay = cm_rep_get_target_ack_delay(rep_msg);
	cm_id_priv->av.timeout =
			cm_ack_timeout(cm_id_priv->target_ack_delay,
				       cm_id_priv->av.timeout - 1);
	cm_id_priv->alt_av.timeout =
			cm_ack_timeout(cm_id_priv->target_ack_delay,
				       cm_id_priv->alt_av.timeout - 1);

	/* todo: handle peer_to_peer */

	ib_cancel_mad(cm_id_priv->av.port->mad_agent, cm_id_priv->msg);
	ret = atomic_inc_and_test(&cm_id_priv->work_count);
	if (!ret)
		list_add_tail(&work->list, &cm_id_priv->work_list);
	spin_unlock_irq(&cm_id_priv->lock);

	if (ret)
		cm_process_work(cm_id_priv, work);
	else
		cm_deref_id(cm_id_priv);
	return 0;

error:
	cm_deref_id(cm_id_priv);
	return ret;
}
