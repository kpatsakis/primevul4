static void cm_format_rep_event(struct cm_work *work, enum ib_qp_type qp_type)
{
	struct cm_rep_msg *rep_msg;
	struct ib_cm_rep_event_param *param;

	rep_msg = (struct cm_rep_msg *)work->mad_recv_wc->recv_buf.mad;
	param = &work->cm_event.param.rep_rcvd;
	param->remote_ca_guid = rep_msg->local_ca_guid;
	param->remote_qkey = be32_to_cpu(rep_msg->local_qkey);
	param->remote_qpn = be32_to_cpu(cm_rep_get_qpn(rep_msg, qp_type));
	param->starting_psn = be32_to_cpu(cm_rep_get_starting_psn(rep_msg));
	param->responder_resources = rep_msg->initiator_depth;
	param->initiator_depth = rep_msg->resp_resources;
	param->target_ack_delay = cm_rep_get_target_ack_delay(rep_msg);
	param->failover_accepted = cm_rep_get_failover(rep_msg);
	param->flow_control = cm_rep_get_flow_ctrl(rep_msg);
	param->rnr_retry_count = cm_rep_get_rnr_retry_count(rep_msg);
	param->srq = cm_rep_get_srq(rep_msg);
	work->cm_event.private_data = &rep_msg->private_data;
}
