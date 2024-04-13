static void cm_format_rep(struct cm_rep_msg *rep_msg,
			  struct cm_id_private *cm_id_priv,
			  struct ib_cm_rep_param *param)
{
	cm_format_mad_hdr(&rep_msg->hdr, CM_REP_ATTR_ID, cm_id_priv->tid);
	rep_msg->local_comm_id = cm_id_priv->id.local_id;
	rep_msg->remote_comm_id = cm_id_priv->id.remote_id;
	cm_rep_set_starting_psn(rep_msg, cpu_to_be32(param->starting_psn));
	rep_msg->resp_resources = param->responder_resources;
	cm_rep_set_target_ack_delay(rep_msg,
				    cm_id_priv->av.port->cm_dev->ack_delay);
	cm_rep_set_failover(rep_msg, param->failover_accepted);
	cm_rep_set_rnr_retry_count(rep_msg, param->rnr_retry_count);
	rep_msg->local_ca_guid = cm_id_priv->id.device->node_guid;

	if (cm_id_priv->qp_type != IB_QPT_XRC_TGT) {
		rep_msg->initiator_depth = param->initiator_depth;
		cm_rep_set_flow_ctrl(rep_msg, param->flow_control);
		cm_rep_set_srq(rep_msg, param->srq);
		cm_rep_set_local_qpn(rep_msg, cpu_to_be32(param->qp_num));
	} else {
		cm_rep_set_srq(rep_msg, 1);
		cm_rep_set_local_eecn(rep_msg, cpu_to_be32(param->qp_num));
	}

	if (param->private_data && param->private_data_len)
		memcpy(rep_msg->private_data, param->private_data,
		       param->private_data_len);
}
